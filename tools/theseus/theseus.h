// Helpers for Theseus rewrites of recompiled guest functions.
//
// A rewrite has the same signature as the generated function it replaces (it may call other
// guest functions with g.call(); see below):
//     void name(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
// and starts with `theseus::Guest g{rdram, ctx, runtime};`. Guest wraps the EE calling
// convention (arguments in a0..a7 = $4..$11, result in v0 = $2, return address in ra = $31)
// and guest memory, using the same READ/WRITE macros as generated code, so hardware
// addresses still go through the runtime.
//
// Rules for rewrites (checked by tools/theseus/difftest.py):
//  - Only touch what the original touches: same memory writes, same return value, same
//    callee-saved registers ($16..$23 s0-s7, $28 gp, $29 sp, $30 fp, $31 ra).
//  - Scratch registers ($1, $3..$15, $24, $25, hi/lo) may end up different, but only if no
//    caller reads them after the call.
//  - Return with g.ret(...) / g.ret_void() and then `return;`: like the generated `jr $ra`,
//    they set ctx->pc to the return address, which the caller's dispatch expects.
#pragma once

#include "ps2_runtime_macros.h"
#include "ps2_runtime.h"
#include "runtime/ee_native_calls.h"

namespace theseus
{
    struct Guest
    {
        uint8_t *rdram;
        R5900Context *ctx;
        PS2Runtime *runtime;
        uint64_t entry_ra;  // our return address; calls overwrite $ra, ret_void() restores it

        Guest(uint8_t *rdram_, R5900Context *ctx_, PS2Runtime *runtime_)
            : rdram(rdram_), ctx(ctx_), runtime(runtime_), entry_ra(GPR_U64(ctx_, 31)) {}

        // Call a guest function. `return_address` is the original's return address for this call
        // site (the instruction after its `jal` + delay slot): the callee sees the same $ra, and
        // in the game the scheduler resumes this rewrite when the callee returns there. Returns
        // v0. Scratch registers ($1, $3..$15, $24, $25, hi/lo) do not survive a call.
        // A rewrite that calls must be shipped through runOnFiber (ship.py does this).
        uint32_t call(uint32_t target, uint32_t return_address)
        {
            ctx = ps2x::native::callGuest(*runtime, rdram, ctx, target, return_address);
            return GPR_U32(ctx, 2);
        }
        // Set argument i (a0..a7) to a 32-bit value, sign-extended as `addiu`/`lw` leave it.
        void set_arg(int i, uint32_t value) { SET_GPR_S32(ctx, 4 + i, value); }
        // Set argument i to a full 64-bit value (e.g. pass an incoming argument through, as `move`).
        void set_arg64(int i, uint64_t value) { SET_GPR_U64(ctx, 4 + i, value); }

        // Arguments: a0..a7 ($4..$11), low 32 bits (pointers and ints on the EE are 32-bit).
        uint32_t arg(int i) const { return GPR_U32(ctx, 4 + i); }
        int32_t sarg(int i) const { return GPR_S32(ctx, 4 + i); }
        // Full 64-bit register, for code that compares or combines whole registers
        // (`beq`, `sltiu`, `and` on 64 bits) rather than their low 32 bits.
        uint64_t arg64(int i) const { return GPR_U64(ctx, 4 + i); }
        int64_t sarg64(int i) const { return GPR_S64(ctx, 4 + i); }
        uint32_t gp() const { return GPR_U32(ctx, 28); }

        // Guest memory (little-endian, like the PS2).
        uint8_t read8(uint32_t addr) const { return READ8(addr); }
        uint16_t read16(uint32_t addr) const { return READ16(addr); }
        uint32_t read32(uint32_t addr) const { return READ32(addr); }
        uint64_t read64(uint32_t addr) const { return READ64(addr); }
        int8_t sread8(uint32_t addr) const { return static_cast<int8_t>(READ8(addr)); }
        int16_t sread16(uint32_t addr) const { return static_cast<int16_t>(READ16(addr)); }
        void write8(uint32_t addr, uint8_t v) const { WRITE8(addr, v); }
        void write16(uint32_t addr, uint16_t v) const { WRITE16(addr, v); }
        void write32(uint32_t addr, uint32_t v) const { WRITE32(addr, v); }
        void write64(uint32_t addr, uint64_t v) const { WRITE64(addr, v); }

        // Return a 32-bit value in v0 (sign-extended to 64 bits, as EE 32-bit ops leave it).
        void ret(uint32_t value) const
        {
            SET_GPR_S32(ctx, 2, value);
            ret_void();
        }
        // Return a value that the original copies with `daddu v0, aN, $zero` (all 64 bits).
        void ret_copy_of_arg(int i) const
        {
            SET_GPR_U64(ctx, 2, GPR_U64(ctx, 4 + i));
            ret_void();
        }
        void ret_void() const
        {
            // `ld $ra` + `jr $ra`: restore our return address (calls overwrite it) and return
            // there (the Return dispatch in generated code is only compiled with
            // PS2X_STRICT_RETURN_DIAGNOSTICS).
            SET_GPR_U64(ctx, 31, entry_ra);
            ctx->pc = static_cast<uint32_t>(entry_ra);
        }
    };
}

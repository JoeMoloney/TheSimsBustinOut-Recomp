// Helpers for hand-written Theseus rewrites of recompiled guest functions.
//
// A rewrite has the same signature as the generated function it replaces:
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

namespace theseus
{
    struct Guest
    {
        uint8_t *rdram;
        R5900Context *ctx;
        PS2Runtime *runtime;

        // Arguments: a0..a7 ($4..$11), low 32 bits (pointers and ints on the EE are 32-bit).
        uint32_t arg(int i) const { return GPR_U32(ctx, 4 + i); }
        int32_t sarg(int i) const { return GPR_S32(ctx, 4 + i); }
        uint32_t gp() const { return GPR_U32(ctx, 28); }

        // Guest memory (little-endian, like the PS2).
        uint8_t read8(uint32_t addr) const { return READ8(addr); }
        uint16_t read16(uint32_t addr) const { return READ16(addr); }
        uint32_t read32(uint32_t addr) const { return READ32(addr); }
        int8_t sread8(uint32_t addr) const { return static_cast<int8_t>(READ8(addr)); }
        int16_t sread16(uint32_t addr) const { return static_cast<int16_t>(READ16(addr)); }
        void write8(uint32_t addr, uint8_t v) const { WRITE8(addr, v); }
        void write16(uint32_t addr, uint16_t v) const { WRITE16(addr, v); }
        void write32(uint32_t addr, uint32_t v) const { WRITE32(addr, v); }

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
            // Exactly what generated code does for `jr $ra` (the Return dispatch it also
            // contains is only compiled with PS2X_STRICT_RETURN_DIAGNOSTICS).
            ctx->pc = GPR_U32(ctx, 31);
        }
    };
}

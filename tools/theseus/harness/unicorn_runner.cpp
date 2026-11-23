// Independent reference for the diff-test harness: runs the ORIGINAL MIPS machine code of a game
// function in Unicorn (QEMU's CPU emulator as a library) instead of a rewrite, so
//     difftest.py <name> --unicorn
// compares the recompiled C++ (PS2Recomp's translation) against the machine code itself. A
// mismatch means the recompiler (or its runtime macros) translates an instruction differently
// from a real MIPS CPU: the kind of bug the normal gate cannot see, because it uses the
// recompiled code as its reference.
//
// CPU model: MIPS64 5Kc. The PS2's R5900 is MIPS III plus movz/movn/pref (MIPS IV) plus its own
// extensions; 5Kc runs MIPS III/IV code natively. R5900 differences handled here:
//   - 3-operand `mult/multu rd, rs, rt` (reserved on 5Kc): emulated in the exception hook.
// NOT modelled (functions using them are out of scope for now; difftest.py warns): the R5900
// FPU (single precision only, no infinities/denormals, round toward zero), MMI and 128-bit
// LQ/SQ, COP2, hardware registers (runtime Load/Store), the upper 64 bits of GPRs.
//
// Calls out of the function are simulated exactly as the stub simulates the recompiled code's
// calls (callGuest in runtime_stub.cpp), so both sides see the same callee effects and produce
// the same event fingerprint.
//
// Skipped cases (theseus_stub::g_skip_reason; the harness counts them and compares nothing):
//   - the random return address lies inside the function (the harness's choice, never real);
//   - the machine code faults on a real CPU: address error (unaligned access) or an address
//     outside the 32 MiB of RAM below the hardware range. Such an input would crash a real
//     PS2, so it is not one the original can receive. (The recompiled code carries on silently.)
// Faults elsewhere (hardware registers, scratchpad, other exceptions) are NOT skipped: they show
// up as event mismatches, since they are gaps in this model, not invalid inputs.
#include "ps2_runtime_macros.h"
#include "ps2_runtime.h"
#include "runtime/ee_native_calls.h"
#include "runtime_stub.h"

#include <unicorn/unicorn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace
{
    constexpr uint64_t kMaxInsns = 20'000'000;  // a runaway loop counts as a failure
    constexpr uint32_t kExcpAdEL = 12, kExcpAdES = 13, kExcpRI = 20, kExcpTLBL = 26, kExcpTLBS = 27;  // QEMU's MIPS exception numbers

    struct Run
    {
        uint8_t *rdram;
        R5900Context *ctx;
        PS2Runtime *runtime;
        uint32_t start, end;
        uint64_t until;          // the return address: Unicorn stops when pc reaches it
        uint64_t last_pc = 0;    // for fault reports (Unicorn's pc is the exception vector by then)
        uint64_t prev_pc = 1;    // the instruction before last_pc (tells a delay slot from a branch target)
        const char *skip = nullptr;
        bool tail_jumped = false;
        bool fault = false;
        uint64_t hi1 = 0, lo1 = 0;  // the R5900's second multiplier pipeline (not in the 5Kc)
    };

    constexpr uint32_t kPage = 4096;

    uc_engine *g_uc = nullptr;
    uc_context *g_clean = nullptr;  // CPU state before any case ran (see theseus_unicorn)
    bool g_trace = false;
    // Pages written in the current case, so that a skipped case undoes exactly those (copying
    // all 32 MiB instead made skips cost ~15 ms each, which dominated whole runs).
    std::vector<uint8_t> g_dirty(PS2_RAM_SIZE / kPage);
    std::vector<uint32_t> g_dirty_list;
    uint8_t *g_mapped = nullptr;
    Run *g_run = nullptr;
    long g_reports = 0;

    void mix(uint64_t value)  // same FNV-1a step as runtime_stub.cpp
    {
        theseus_stub::g_events.hash = (theseus_stub::g_events.hash ^ value) * 0x100000001B3ull;
    }

    void check(uc_err e, const char *what)
    {
        if (e != UC_ERR_OK)
        {
            std::fprintf(stderr, "unicorn: %s: %s\n", what, uc_strerror(e));
            std::exit(3);
        }
    }

    void regs_to_uc(uc_engine *uc, R5900Context *ctx)
    {
        for (int r = 1; r < 32; ++r)
        {
            uint64_t v = GPR_U64(ctx, r);
            uc_reg_write(uc, UC_MIPS_REG_0 + r, &v);
        }
        uc_reg_write(uc, UC_MIPS_REG_HI, &ctx->hi);
        uc_reg_write(uc, UC_MIPS_REG_LO, &ctx->lo);
    }

    void regs_from_uc(uc_engine *uc, R5900Context *ctx)
    {
        for (int r = 1; r < 32; ++r)
        {
            uint64_t v = 0;
            uc_reg_read(uc, UC_MIPS_REG_0 + r, &v);
            Ps2SetGprLow64(ctx, r, _mm_cvtsi64_si128(int64_t(v)));  // upper 64 bits untouched
        }
        uc_reg_read(uc, UC_MIPS_REG_HI, &ctx->hi);
        uc_reg_read(uc, UC_MIPS_REG_LO, &ctx->lo);
    }

    // Instructions of the function under test whose meaning on the 5Kc differs from the R5900,
    // replaced in RAM by a reserved opcode so that they trap and are emulated (emulate_r5900).
    // Without this some run natively and silently compute something else:
    //   - SPECIAL mult/multu rd, rs, rt: the 5Kc reads rd as a DSP accumulator number (rd & 3):
    //     rd = 4, 8, ... is accumulator 0, a plain mult that does not write rd (others trap);
    //   - the MMI opcode 0x1C is the 5Kc's SPECIAL2: madd1 would run as clz, plzcw as msub, ...
    //   - lq / sq (0x1E / 0x1F; used to save and restore the 128-bit callee-saved registers).
    // Both RAM images are patched identically (the recompiled side never reads its code), so the
    // harness's RAM compare does not see it. address -> original word:
    std::unordered_map<uint32_t, uint32_t> g_patched;
    constexpr uint32_t kTrapWord = 0x78000000u;  // opcode 0x1E (MDMX): reserved on the 5Kc

    bool differs_on_5kc(uint32_t w)
    {
        const uint32_t op = w >> 26, funct = w & 0x3F, rd = (w >> 11) & 31;
        return (op == 0 && (funct == 0x18 || funct == 0x19) && rd != 0) || op == 0x1C
            || op == 0x1E || op == 0x1F;  // lq / sq (the 5Kc runs them as something else, silently)
    }

    uint8_t *g_ram_for_insn = nullptr;

    uint32_t insn_at(uint64_t address)
    {
        const uint32_t a = uint32_t(address) & (PS2_RAM_SIZE - 1);
        if (const auto it = g_patched.find(a); it != g_patched.end())
            return it->second;
        uint32_t w;
        std::memcpy(&w, g_ram_for_insn + a, 4);
        return w;
    }

    void mark_store(uc_engine *uc, uint32_t w);
    void unmapped_access(uint64_t address);

    // Every instruction inside the function (cheap: the functions under test are small).
    void on_code(uc_engine *uc, uint64_t address, uint32_t, void *)
    {
        Run &run = *g_run;
        run.prev_pc = run.last_pc;
        run.last_pc = address;
        const uint32_t w = insn_at(address);
        if (g_trace)
        {
            uint64_t a0, a1, a2, ra;
            uc_reg_read(uc, UC_MIPS_REG_4, &a0); uc_reg_read(uc, UC_MIPS_REG_5, &a1);
            uc_reg_read(uc, UC_MIPS_REG_6, &a2); uc_reg_read(uc, UC_MIPS_REG_31, &ra);
            std::fprintf(stderr, "  trace %llx: %08x a0=%llx a1=%llx a2=%llx ra=%llx\n", (unsigned long long)address, w,
                         (unsigned long long)a0, (unsigned long long)a1, (unsigned long long)a2, (unsigned long long)ra);
        }
        mark_store(uc, w);
    }

    // A block outside the function = a call (or tail jump) to another function.
    void on_block(uc_engine *uc, uint64_t address, uint32_t, void *)
    {
        if (!g_run) return;  // the pending-branch nop
        Run &run = *g_run;
        const uint32_t pc = uint32_t(address);
        if ((pc >= run.start && pc < run.end) || address == run.until)
            return;
        if (g_trace)
        {
            uint64_t st = 0;
            uc_reg_read(uc, UC_MIPS_REG_CP0_STATUS, &st);
            std::fprintf(stderr, "  block out %llx status %llx\n", (unsigned long long)address, (unsigned long long)st);
        }
        regs_from_uc(uc, run.ctx);
        const uint32_t ra = GPR_U32(run.ctx, 31);
        // jal/jalr set ra to an address inside the function; a tail jump leaves the caller's ra.
        const bool call = ra >= run.start && ra < run.end;
        ps2x::native::callGuest(*run.runtime, run.rdram, run.ctx, pc, ra);  // same simulation as the stub
        regs_to_uc(uc, run.ctx);
        uint64_t next = int64_t(int32_t(ra));
        uc_reg_write(uc, UC_MIPS_REG_PC, &next);
        if (!call)
        {
            run.tail_jumped = true;
            uc_emu_stop(uc);
        }
    }

    void mark(uint64_t address)
    {
        const uint32_t page = (uint32_t(address) & (PS2_RAM_SIZE - 1)) / kPage;
        if (!g_dirty[page])
        {
            g_dirty[page] = 1;
            g_dirty_list.push_back(page);
        }
    }

    // Stores are tracked by decoding them in the code hook, not with UC_HOOK_MEM_WRITE: with a
    // memory hook, QEMU loses track of a taken branch-likely whose delay slot is a store and
    // raises "reserved instruction" on the next branch (seen as exception 20 on `jr $ra`).
    void mark_store(uc_engine *uc, uint32_t w)
    {
        switch (w >> 26)
        {
        case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2E:  // sb sh swl sw sdl sdr swr
        case 0x3F: case 0x1F: case 0x39: case 0x3D: case 0x3E:                        // sd sq swc1 sdc1 sqc2
        {
            uint64_t base = 0;
            uc_reg_read(uc, UC_MIPS_REG_0 + ((w >> 21) & 31), &base);
            const uint64_t address = base + int64_t(int16_t(w & 0xFFFF));
            mark(address);
            mark(address + 15);
            break;
        }
        default: break;
        }
    }

    // Puts back every page this case wrote (stores here, callee writes into the window) from the
    // untouched input RAM.
    void restore(uint8_t *rdram)
    {
        const uint8_t *pristine = theseus_stub::g_pristine;
        if (!pristine)
        {
            std::fprintf(stderr, "unicorn: no pristine RAM to restore a skipped case from\n");
            std::exit(3);
        }
        for (uint32_t page : g_dirty_list)
            std::memcpy(rdram + page * kPage, pristine + page * kPage, kPage);
        std::memcpy(rdram + theseus_stub::kWindow, pristine + theseus_stub::kWindow, theseus_stub::kWindowSize);
    }

    // R5900 instructions the 5Kc model rejects as reserved, emulated here. The faulting
    // instruction is the code hook's last pc (inside the hook Unicorn's pc reads 0); Unicorn does
    // not enter a guest exception handler, so writing pc = that + 4 resumes after it. (Not
    // handled: one in a branch delay slot; it stays an exception and so a mismatch.)
    //   SPECIAL mult/multu rd, rs, rt   5Kc decodes a nonzero rd as a DSP accumulator (no DSP ASE)
    //   MMI mult1/multu1, madd/maddu, madd1/maddu1, mfhi1/mflo1/mthi1/mtlo1
    // R5900 multiply: a 64-bit product of the low 32 bits of rs and rt (plus HI:LO for madd);
    // LO/HI = its low/high halves, each sign-extended; rd = LO.
    bool is_branch(uint32_t w)
    {
        const uint32_t op = w >> 26;
        return (op >= 0x02 && op <= 0x07) || (op >= 0x14 && op <= 0x17) || op == 0x01
            || (op == 0 && ((w & 0x3F) == 0x08 || (w & 0x3F) == 0x09));
    }

    // The pc after the delay slot of the branch at `at`, evaluated on the current registers.
    // Branches that link (jal, jalr, bltzal...) are not handled: false.
    bool branch_target(uc_engine *uc, uint64_t at, uint64_t &next)
    {
        const uint32_t w = insn_at(at), op = w >> 26, rt = (w >> 16) & 31;
        auto reg = [&](uint32_t r) { int64_t v = 0; uc_reg_read(uc, UC_MIPS_REG_0 + r, &v); return v; };
        const int64_t a = reg((w >> 21) & 31), b = reg(rt);
        const uint64_t taken = at + 4 + (int64_t(int16_t(w & 0xFFFF)) << 2);
        bool cond;
        switch (op)
        {
        case 0x04: case 0x14: cond = a == b; break;  // beq(l)
        case 0x05: case 0x15: cond = a != b; break;  // bne(l)
        case 0x06: case 0x16: cond = a <= 0; break;  // blez(l)
        case 0x07: case 0x17: cond = a > 0; break;   // bgtz(l)
        case 0x01:
            if (rt == 0x00 || rt == 0x02) { cond = a < 0; break; }   // bltz(l)
            if (rt == 0x01 || rt == 0x03) { cond = a >= 0; break; }  // bgez(l)
            return false;
        case 0x02: next = (at & ~0x0FFFFFFFull) | ((w & 0x03FFFFFF) << 2); return true;  // j
        case 0x00:
            if ((w & 0x3F) == 0x08) { next = uint64_t(a); return true; }  // jr
            return false;
        default: return false;
        }
        next = cond ? taken : at + 8;
        return true;
    }

    // Continue at next_pc. After an exception in a delay slot QEMU still has the branch pending
    // (hidden state), so then the clean CPU state is restored around the registers first.
    bool resume_after(uc_engine *uc, uint64_t next_pc, bool in_delay)
    {
        if (in_delay)
        {
            uint64_t r[34];
            for (int i = 1; i < 32; ++i) uc_reg_read(uc, UC_MIPS_REG_0 + i, &r[i]);
            uc_reg_read(uc, UC_MIPS_REG_HI, &r[32]);
            uc_reg_read(uc, UC_MIPS_REG_LO, &r[33]);
            check(uc_context_restore(uc, g_clean), "context restore (delay slot)");
            for (int i = 1; i < 32; ++i) uc_reg_write(uc, UC_MIPS_REG_0 + i, &r[i]);
            uc_reg_write(uc, UC_MIPS_REG_HI, &r[32]);
            uc_reg_write(uc, UC_MIPS_REG_LO, &r[33]);
        }
        uc_reg_write(uc, UC_MIPS_REG_PC, &next_pc);
        return true;
    }

    bool emulate_r5900(uc_engine *uc)
    {
        Run &run = *g_run;
        uint64_t pc = run.last_pc;
        const uint32_t w = insn_at(pc);
        const uint32_t op = w >> 26, funct = w & 0x3F, sa = (w >> 6) & 31;
        const uint32_t rs = (w >> 21) & 31, rt = (w >> 16) & 31, rd = (w >> 11) & 31;
        auto get = [&](int r) { uint64_t v = 0; uc_reg_read(uc, UC_MIPS_REG_0 + r, &v); return v; };
        auto set = [&](int r, uint64_t v) { if (r) uc_reg_write(uc, UC_MIPS_REG_0 + r, &v); };
        auto sx32 = [](uint64_t v) { return uint64_t(int64_t(int32_t(uint32_t(v)))); };
        uint64_t hi0 = 0, lo0 = 0;
        uc_reg_read(uc, UC_MIPS_REG_HI, &hi0);
        uc_reg_read(uc, UC_MIPS_REG_LO, &lo0);
        // Where execution continues, decided before this instruction changes any register (in a
        // delay slot the branch was evaluated before it).
        uint64_t next_pc = pc + 4;
        const bool in_delay = run.prev_pc == pc - 4 && is_branch(insn_at(pc - 4));
        if (in_delay && !branch_target(uc, pc - 4, next_pc))
            return false;  // a branch kind not handled below: stays an exception (a mismatch)

        bool pipe1 = false, accumulate = false, is_signed = false;
        if (op == 0 && (funct == 0x18 || funct == 0x19) && sa == 0)
            is_signed = funct == 0x18;
        else if (op == 0x1C && sa == 0 && (funct == 0x18 || funct == 0x19))
            pipe1 = true, is_signed = funct == 0x18;
        else if (op == 0x1C && sa == 0 && (funct == 0x00 || funct == 0x01 || funct == 0x20 || funct == 0x21))
            pipe1 = funct >= 0x20, accumulate = true, is_signed = (funct & 1) == 0;
        else if (op == 0x1E || op == 0x1F)
        {
            // lq / sq rt, off(base): 128 bits at (base + off) with the low 4 bits ignored. Unicorn's
            // registers are 64-bit; the upper halves live in the context, which nothing else here
            // writes (the simulated callee and regs_from_uc only set low halves).
            const uint32_t address = uint32_t(get(int(rs)) + int64_t(int16_t(w & 0xFFFF))) & ~15u;
            const uint32_t p = address >= 0x80000000u && address < 0xC0000000u ? address & 0x1FFFFFFFu : address;
            const bool in_ram = (p < PS2_RAM_SIZE) || (p >= 0x20000000u && p < 0x20000000u + PS2_RAM_SIZE)
                             || (p >= 0x30000000u && p < 0x30000000u + PS2_RAM_SIZE);
            if (!in_ram)
            {
                unmapped_access(address);
                uc_emu_stop(uc);
                return true;
            }
            uint8_t *mem = run.rdram + (p & (PS2_RAM_SIZE - 1));
            uint8_t *upper = reinterpret_cast<uint8_t *>(&run.ctx->r[rt]) + 8;
            if (op == 0x1E)
            {
                if (rt != 0)
                {
                    uint64_t low;
                    std::memcpy(&low, mem, 8);
                    set(int(rt), low);
                    std::memcpy(upper, mem + 8, 8);
                }
            }
            else
            {
                const uint64_t low = get(int(rt));
                std::memcpy(mem, &low, 8);
                if (rt != 0)
                    std::memcpy(mem + 8, upper, 8);
                else
                    std::memset(mem + 8, 0, 8);
            }
            return resume_after(uc, next_pc, in_delay);
        }
        else if (op == 0x1C && (funct >= 0x10 && funct <= 0x13))
        {
            switch (funct)
            {
            case 0x10: set(int(rd), run.hi1); break;   // mfhi1
            case 0x11: run.hi1 = get(int(rs)); break;  // mthi1
            case 0x12: set(int(rd), run.lo1); break;   // mflo1
            case 0x13: run.lo1 = get(int(rs)); break;  // mtlo1
            }
            return resume_after(uc, next_pc, in_delay);
        }
        else
            return false;

        const uint64_t a = get(int(rs)), b = get(int(rt));
        uint64_t product = is_signed ? uint64_t(int64_t(int32_t(a)) * int64_t(int32_t(b)))
                                     : uint64_t(uint32_t(a)) * uint64_t(uint32_t(b));
        if (accumulate)
        {
            const uint64_t hi = pipe1 ? run.hi1 : hi0, lo = pipe1 ? run.lo1 : lo0;
            product += (uint64_t(uint32_t(hi)) << 32) | uint32_t(lo);
        }
        const uint64_t lo = sx32(product), hi = sx32(product >> 32);
        if (pipe1)
            run.hi1 = hi, run.lo1 = lo;
        else
        {
            uc_reg_write(uc, UC_MIPS_REG_LO, &lo);
            uc_reg_write(uc, UC_MIPS_REG_HI, &hi);
        }
        set(int(rd), lo);
        return resume_after(uc, next_pc, in_delay);
    }

    void report(const char *what, uint64_t address)
    {
        if (++g_reports <= 5)
            std::fprintf(stderr, "unicorn: %s, last pc 0x%llx insn %08x, address 0x%llx\n", what,
                         (unsigned long long)g_run->last_pc, insn_at(g_run->last_pc), (unsigned long long)address);
    }

    // RAM is mapped at 0 and at its uncached mirrors 0x20000000 / 0x30000000 (kseg0/kseg1
    // reach it through physical 0). Of the rest, these exist on a PS2 but are not modelled here.
    // The functions checked are ones the inventory lists without hardware accesses
    // (unicorn_check.py; difftest.py warns otherwise), so a pointer into these comes from the
    // random input: skipped as well, with its own reason.
    bool exists_on_ps2(uint32_t a)
    {
        const uint32_t p = a >= 0x80000000u && a < 0xC0000000u ? a & 0x1FFFFFFFu : a;  // kseg0/1
        return (p >= 0x10000000u && p < 0x10010000u)     // EE hardware registers
            || (p >= 0x11000000u && p < 0x11010000u)     // VU0/VU1 memory
            || (p >= 0x12000000u && p < 0x12002000u)     // GS privileged registers
            || (p >= 0x1C000000u && p < 0x1C200000u)     // IOP RAM
            || (p >= 0x1FC00000u && p < 0x20000000u)     // BIOS ROM
            || (a >= 0x70000000u && a < 0x70004000u);    // scratchpad
    }

    // An access to an address with nothing behind it: a real PS2 faults (TLB miss or bus error),
    // so the input is one the function cannot receive and the case is skipped.
    void unmapped_access(uint64_t address)
    {
        Run &run = *g_run;
        run.skip = exists_on_ps2(uint32_t(address))
                       ? "input pointer into hardware/IOP/BIOS/scratchpad (not modelled; not RAM data)"
                       : "address with no memory behind it (a real PS2 would fault)";
        run.fault = true;
    }

    // The data address of the load/store at pc (base register + offset).
    uint64_t effective_address(uc_engine *uc, uint64_t pc)
    {
        const uint32_t w = insn_at(pc);
        uint64_t base = 0;
        uc_reg_read(uc, UC_MIPS_REG_0 + ((w >> 21) & 31), &base);
        return base + int64_t(int16_t(w & 0xFFFF));
    }

    bool on_unmapped(uc_engine *uc, uc_mem_type type, uint64_t address, int, int64_t, void *)
    {
        if (!g_run) return false;
        Run &run = *g_run;
        if (type != UC_MEM_FETCH_UNMAPPED)
        {
            unmapped_access(address);
            uc_emu_stop(uc);
            return false;
        }
        else
            report(type == UC_MEM_WRITE_UNMAPPED ? "unmapped write" : type == UC_MEM_FETCH_UNMAPPED ? "unmapped fetch"
                                                                                                 : "unmapped read",
                   address);
        run.fault = true;
        uc_emu_stop(uc);
        return false;
    }

    void on_interrupt(uc_engine *uc, uint32_t intno, void *)
    {
        if (!g_run) { uc_emu_stop(uc); return; }
        Run &run = *g_run;
        if (g_trace)
        {
            uint64_t pc = 0;
            uc_reg_read(uc, UC_MIPS_REG_PC, &pc);
            std::fprintf(stderr, "  interrupt %u pc %llx\n", intno, (unsigned long long)pc);
        }
        if (intno == kExcpRI && emulate_r5900(uc))
            return;  // carry on after the instruction
        if (intno == kExcpAdEL || intno == kExcpAdES)
            run.skip = "address error (unaligned access: a real PS2 would fault)";
        else if (intno == kExcpTLBL || intno == kExcpTLBS)  // kuseg outside the mapped RAM
            unmapped_access(effective_address(uc, run.last_pc));
        else
        {
            // syscall / break / trap / reserved instruction / overflow: not what the recompiled
            // code's runtime does, so the case shows up as an event mismatch.
            ++theseus_stub::g_events.exceptions;
            mix(0xE0000000ull | intno);
            char what[48];
            std::snprintf(what, sizeof what, "CPU exception %u", intno);
            report(what, 0);
        }
        run.fault = true;
        uc_emu_stop(uc);
    }

    void setup(uint8_t *rdram, uint32_t start, uint32_t end)
    {
        if (!g_uc)
        {
            check(uc_open(UC_ARCH_MIPS, uc_mode(UC_MODE_MIPS64 | UC_MODE_LITTLE_ENDIAN), &g_uc), "open");
            check(uc_ctl_set_cpu_model(g_uc, UC_CPU_MIPS64_5KC), "cpu model");
            uc_hook h;
            check(uc_hook_add(g_uc, &h, UC_HOOK_CODE, (void *)on_code, nullptr, start, end - 1), "code hook");
            check(uc_hook_add(g_uc, &h, UC_HOOK_BLOCK, (void *)on_block, nullptr, 1, 0), "block hook");
            check(uc_hook_add(g_uc, &h, UC_HOOK_MEM_UNMAPPED, (void *)on_unmapped, nullptr, 1, 0), "mem hook");
            check(uc_hook_add(g_uc, &h, UC_HOOK_INTR, (void *)on_interrupt, nullptr, 1, 0), "intr hook");
            g_trace = std::getenv("UNICORN_TRACE") != nullptr;
            if (std::getenv("UNICORN_TRACE_ALL"))  // every instruction anywhere, raw
                check(uc_hook_add(g_uc, &h, UC_HOOK_CODE, (void *)+[](uc_engine *, uint64_t a, uint32_t, void *) {
                    std::fprintf(stderr, "  any %llx: %08x\n", (unsigned long long)a, a < PS2_RAM_SIZE ? insn_at(a) : 0u);
                }, nullptr, 1, 0), "trace-all hook");
            check(uc_context_alloc(g_uc, &g_clean), "context alloc");
            check(uc_context_save(g_uc, g_clean), "context save");
        }
        if (g_mapped != rdram)
        {
            if (g_mapped)
                for (uint64_t at : {0x00000000ull, 0x20000000ull, 0x30000000ull})
                    check(uc_mem_unmap(g_uc, at, PS2_RAM_SIZE), "unmap");
            // Zero copy: Unicorn runs directly on the harness's RAM image.
            for (uint64_t at : {0x00000000ull, 0x20000000ull, 0x30000000ull})  // RAM and its mirrors
                check(uc_mem_map_ptr(g_uc, at, PS2_RAM_SIZE, UC_PROT_ALL, rdram), "map RAM");
            g_ram_for_insn = rdram;
            g_patched.clear();
            uint8_t *pristine = const_cast<uint8_t *>(theseus_stub::g_pristine);
            for (uint32_t a = start; a < end; a += 4)
            {
                uint32_t w;
                std::memcpy(&w, rdram + a, 4);
                if (!differs_on_5kc(w))
                    continue;
                g_patched[a] = w;
                std::memcpy(rdram + a, &kTrapWord, 4);
                if (pristine)
                    std::memcpy(pristine + a, &kTrapWord, 4);
            }
            g_mapped = rdram;
        }
    }
}

void theseus_unicorn(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime, uint32_t start, uint32_t end)
{
    setup(rdram, start, end);
    const uint32_t ra = GPR_U32(ctx, 31);
    if (ra >= start && ra < end)
    {
        // Unicorn's stop-at-address check would also fire on the function's own branch there.
        theseus_stub::g_skip_reason = "return address inside the function (random input artifact)";
        return;
    }
    for (uint32_t a = start; a < end; a += 4)
    {
        // Likewise for a call or jump target: Unicorn would stop at the call instead of simulating it.
        const uint32_t w = insn_at(a);
        if ((w >> 26 == 0x02 || w >> 26 == 0x03) && ((a & 0xF0000000u) | ((w & 0x03FFFFFFu) << 2)) == ra)
        {
            theseus_stub::g_skip_reason = "return address equals a call target (random input artifact)";
            return;
        }
    }
    for (uint32_t page : g_dirty_list)
        g_dirty[page] = 0;
    g_dirty_list.clear();
    Run run{rdram, ctx, runtime, start, end, uint64_t(int64_t(int32_t(ra)))};
    run.hi1 = ctx->hi1;
    run.lo1 = ctx->lo1;
    g_run = &run;
    // Every case starts from the CPU state saved before the first one. QEMU keeps hidden state
    // between uc_emu_start calls that the register API cannot reach: after stopping at `until`
    // right behind a `jr $ra` the branch is still pending (the next case's first instruction ran
    // as its delay slot and "returned" to the previous case's return address), and after an
    // exception in a delay slot an ordinary store later raised "reserved instruction".
    check(uc_context_restore(g_uc, g_clean), "context restore");
    regs_to_uc(g_uc, ctx);
    if (g_trace)
    {
        uint64_t st = 0;
        uc_reg_read(g_uc, UC_MIPS_REG_CP0_STATUS, &st);
        std::fprintf(stderr, "  start status %llx\n", (unsigned long long)st);
    }
    const uc_err err = uc_emu_start(g_uc, start, run.until, 0, kMaxInsns);
    uint64_t pc = 0;
    uc_reg_read(g_uc, UC_MIPS_REG_PC, &pc);
    regs_from_uc(g_uc, ctx);
    ctx->hi1 = run.hi1;
    ctx->lo1 = run.lo1;
    ctx->pc = uint32_t(pc);
    if (run.skip)
    {
        theseus_stub::g_skip_reason = run.skip;
        restore(rdram);
    }
    else if (err != UC_ERR_OK && !run.fault)
    {
        report(uc_strerror(err), 0);
        ++theseus_stub::g_events.exceptions;
        mix(0xEE000000ull | err);
    }
    else if (!run.fault && !run.tail_jumped && pc != run.until)  // the instruction limit
    {
        report("instruction limit reached (runaway loop?)", 0);
        ++theseus_stub::g_events.exceptions;
        mix(0xEF000000ull);
    }
    // A normal return records no event: the generated code's `jr $ra` only sets pc (its Return
    // dispatch exists only with PS2X_STRICT_RETURN_DIAGNOSTICS).
    g_run = nullptr;
}

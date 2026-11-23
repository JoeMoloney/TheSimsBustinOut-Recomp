# 2026-11-23: Unicorn reference, class recovery, PS2 FPU

Moved out of HANDOFF.md (Nov 23). Covers the Unicorn reference in the diff-test harness, the
vtable class map and the PS2 FPU model, plus the recompiler bugs these found.

## Class recovery from vtables

- `python3 tools/classes/vtables.py` (under 1 s; reports in `out/classes/classes.{tsv,md}`).
- Old GCC 2.9x ABI: 8-byte entries `{int16 delta, int16 index, fn}`, a zero header entry, methods
  from vt+8, the vptr anywhere in the object (+0, +0x10, +0x44 most common).
- Found from constructor/destructor stores (symbolic `this` / `operator new(N)` tracking):
  464 classes, 3,451 virtual methods, 264 constructors, 155 base links, 210 object sizes. Calls
  with a0 = this+k are members, not bases.
- Biggest family: root `0x3df898` (vptr +0x44, 68 classes). Known gaps: containers that construct
  members inline show several vptr offsets (e.g. `0x3abfe0`); the common root `0x3f08a0`
  (3 methods, ctor always inlined).
- Names: no universal GetClassName method. Only 5 classes return a string: `Object Manager`,
  `Action Queue Manager`, `Server Responder`, the app (`PS2 Sims Bustin Out Build 2.10.12.19-1`)
  and the engine (`EOR Engine v2.0 built ... Dec 19 2003`).
- Idea, not done: source-file (translation unit) boundaries. Header strings are emitted once per
  .cpp that includes them (`EStorable, EResource, EFontCharacter, ..., ../../../engine/e_texture.h`
  appears ~230 times in .rodata, in include order). String literals are private to their .cpp and
  .text/.rodata follow the same link order, so the .rodata block a function references gives its
  source file; the include list in each block tells which headers (classes) that file uses.

## Unicorn reference (`difftest.py <name> --unicorn`)

Runs the original machine code in Unicorn (`tools/theseus/harness/unicorn_runner.cpp`, zero-copy
on the harness RAM) against the recompiled C++. Recompiler bugs can't pass this check, unlike the
normal gate, which compares a rewrite against the recompiled code.

- First ledger run (MIPS64 5Kc, no FPU): 628 of 684 MATCH (every register incl. scratch, all
  memory), 55 ABI-ONLY (all the movz/movn bug), 1 MISMATCH (invalid input: `Str_ToWstr16N` with
  n=-1 and no terminator; the recompiled code wraps around RAM, a PS2 faults).
- **Runner model** (each item was found by a mismatch):
  - R5900 instructions the MIPS64 core gets wrong are replaced in RAM (both images, identically)
    by a reserved opcode and emulated in the trap: `mult/multu rd` (the 5Kc reads rd as a DSP
    accumulator), all MMI opcode 0x1C (= SPECIAL2 there: madd1 would run as clz), `lq/sq` (upper
    halves kept in the context), mult1/madd(u)(1), mfhi1/mflo1/mthi1/mtlo1, and every COP1
    instruction except bc1t/bc1f (also in delay slots).
  - Each case starts from a saved clean CPU context (QEMU keeps a pending branch after stopping
    at `until`). Stores are tracked by decoding in the code hook (a UC_HOOK_MEM_WRITE breaks
    branch-likely delay slots in QEMU).
  - Skipped cases (a real PS2 faults, or a random-input artifact): unaligned access, no memory at
    the address, input pointer into hardware/IOP/BIOS, ra inside the function or equal to a call
    target, `break 7` (gcc's divide-by-zero trap), jr/jalr to an address with no RAM (letting
    Unicorn fetch there corrupts its translator and the next run segfaults).
  - Over 2M instructions: skipped (a loop count from random input; the recompiled side has no
    limit, so these used to hit the 600 s timeout).
  - Self-recursion: a `jal` to the function's own start compiles to `goto` its first label, so
    the inner `jr $ra` leaves the C function with pc at the return point; the game's dispatcher
    re-enters through the function's entry `switch`. The Unicorn bind (`difftest.py`) re-enters
    the same way (e.g. `FUN_002be238`, which runs 7,718 times in CAS, now matches).
  - Not modelled (functions excluded): other MMI, COP2, hardware registers.
  - `UNICORN_TRACE=1` traces every case, `UNICORN_TRACE_CASE=n` only case n.
- Random pointers: in pointer-heavy functions up to 98% of cases were unaligned. `window_ptr` in
  `difftest_harness.cpp` now returns a 16-byte-aligned pointer 3 times in 4 (~6x more usable
  cases on `fpu_00103c10`).

## PS2 FPU model (`tools/theseus/harness/ps2_fpu_ref.h`)

The core is now `UC_CPU_MIPS64_5KF` with CU1 set, so `bc1t/bc1f` run natively on FCSR bit 23,
which the runner writes from the context's C bit before every run and after every call out.
Every other COP1 instruction is emulated through `ps2_fpu_ref.h`:

- PS2 rules: no Inf/NaN (exponent 255 is an ordinary binade, Fmax = 0x7FFFFFFF), exponent 0 is
  zero (no denormals), overflow clamps to +/-Fmax, underflow flushes to +/-0. Compares use an
  integer key; max/min compare raw bits as integers. `sqrt.s fd, ft` reads **ft** (fs is 0);
  `rsqrt.s` = fs/sqrt(ft). DIV.S: zero divisor sets D|SD, 0/0 sets I|SI.
- Two modes. `exact` (default): a port of PCSX2 PR #12001's PS2Float (GPL-3.0+, not merged,
  commit 0392a64; the PCSX2 interpreter itself is not bit-exact): the adder's truncation, the
  Booth multiplier, radix-4 SRT division/square root. `UNICORN_FPU=nearest`: IEEE round-to-nearest
  with the PS2 range rules, computed via double (double rounding through >= 50 bits is innocuous
  for + - x / sqrt), which is what the recompiled runtime does.
- Checks (`work/fpu-port-check/`): the port vs PS2Float, 0 mismatches over 100M inputs per
  operation; the port vs the ps2autotests hardware vectors (`ps2xTest/src/ps2_float_hw_vectors.inc`,
  174 rows; the sqrt operand is in the "fs" column), 174/174; the runtime macros vs nearest mode,
  0/20M for all 8 operations after the fixes below.
- Instruction census over the 1,769 FPU functions (`work/cop1_census.py`): no cfc1/ctc1, no
  madd/adda; most common swc1 17k, lwc1 15.7k, mtc1 7.5k, mul 4.6k, add 4.2k ... sqrt 147,
  min 96, abs 56, max 37, rsqrt 1.
- `unicorn_check.py --all-fpu [--tag T]` checks every inventory function with FPU code and only
  modelled features (1,605); `-j 20` default.

## Recompiler/runtime bugs found (PS2Recomp, regenerated Nov 23)

1. **`sqrt.s` read fs instead of ft**: every sqrt computed sqrt(f0). 107 of 147 sqrt instructions
   in 65 functions; 8 are Create-A-Sim-only (0x107ce0, 0x20b970, 0x27ad80, 0x27ce88, 0x27d250,
   0x2b5708, 0x2b6218, 0x2b6320). `FUN_002b5708` (2 wrong sqrts) was already the top CAS float
   suspect (30k calls). Strong 3D lead.
2. FPU compares followed IEEE NaN/denormal rules (now `ps2_fcmp_key`).
3. max/min used float compares (now `ps2_fmax/ps2_fmin`, integer).
4. Division by zero set a bogus FCR31 bit 0x100000 (now `ps2_fdiv_flags`).
5. Runtime add/sub/mul treated exponent-0 operands as tiny numbers (`ps2_fall_normal` in the
   fast paths); e.g. tiny x huge gave -0.13 where the PS2 gives -0.
6. movz/movn copied all 128 bits (`SET_GPR_VEC`); the R5900 moves the low 64 (fixed earlier,
   regenerated with these).

Regeneration: 1,047 main and 40 supplemental files changed; backups
`work/sims-bustin-out/{functions-only-output,supplemental-output}.before-fpu`. `ps2x_tests` 460/460.

Also fixed: difftest build stamps didn't include `ps2_runtime_macros.h` or `ps2_fpu_ref.h`, so a
semantics change could test stale objects.

## Sweep triage and the 3D result

- First nearest-mode pass: 1,273 MATCH, 220 MISMATCH, 112 ERROR. None of the failures was a
  recompiler bug; all were harness gaps, fixed in this order:
  - jr/jalr to an unmapped address (Unicorn segfaulted on the next run): skipped before the jump;
  - random loop counts (78 timeouts): > 2M instructions is a skip;
  - self-recursion (`goto` start + re-entry through the entry switch): the Unicorn bind re-enters;
  - fragments (`dataref_*`) whose b/beq leaves the range or which fall off their end compile to
    `pc = target; return;`, not a call: Unicorn stops there too (a `j` out is a tail call);
  - jump-table `jr` to a label of the same function: the stub (`runtime_stub.cpp`) re-enters
    instead of simulating a tail call (`theseus_stub::g_func_start/end`, set by the Unicorn bind);
  - **Unicorn bug:** redirecting pc from the exception hook after an emulated delay-slot
    instruction left QEMU in a pending-branch state; later-translated blocks could be wrong
    (`addiu s1,s1,0x24` silently did nothing in `fpu_0027e848`'s copy loop). Now the hook stops
    the emulator and the run loop restarts at the branch target from the clean context (a TB
    flush also fixed it but cost 50x).
- After all fixes: every tested FPU function matches; ~20 loop too long on random input (need
  rules). Ledger re-check: 684/684 MATCH.
- **Game rebuilt and run to Create-A-Sim: the 3D is fixed** (room, Sim, parrot, window; frames in
  `work/frames-fpu/`). Before: stretched wedges, flat yellow sky. Most likely the `sqrt.s` operand
  fix; not bisected against the other FPU fixes.

# Sims Bustin' Out Recomp — Handoff

Last updated: 2026-11-23. **Keep this file short** (target ~10 KB): current state, next steps, reference.
Session history goes to `docs/devlog/` (one file per date range; the full pre-Nov-23 handoff is archived there verbatim).

## Current state

- **Boots and plays** into Create-A-Sim and the house (static recompilation with PS2Recomp, HLE for pad/mc/mpeg). Slow (~5-8x below real time; the software GS renderer is ~72% of the game thread).
- **3D is wrong.** **Create-A-Sim and the house look like the camera is inside geometry** (huge stretched wedges, flat yellow sky, no Sim visible). Unchanged by the CVT.W.S fix (Nov 19), the sign-branch fix and the VU1 store fix (Nov 22).
- **Nov 23: the scooter intro is a pre-rendered MOVIE (`sims_intro_pal` in `DATA/MOVIES.ARC`), not real-time 3D.** Call counts (`work/callcounts-1/`): `sceMpegGetPicture` returned 105 pictures and `DemuxPssRing` ran through the whole scooter window, and the camera routine 0x2b1ce0 ran 2 times before the main menu vs 1,346 after. **So Create-A-Sim is the first real-time 3D scene the game draws**; the old reasoning "the intro draws fine, so VU1/GS are fine, suspect EE camera maths" is void: VIF/VU1/GS and EE maths are all suspects again.
- CAS-only code (called after the main menu, never before): 604 functions, 144 with FPU code, **only 5 with real 64-bit arithmetic**, none with COP2 (`work/callcounts-1/cas-new.tsv`). Biggest FPU candidate: `FUN_002b5708` (396 insns, 254 FPU, 30k calls).
- **Theseus** (hand rewrites of recompiled functions, diff-tested against the originals): ledger 683 rows, **470 shipped, 213 gated**. Left: 41 tier-B entries (26-196 insns), 3 deferred (0x289310, 0x365278 self-recursive; `_fpadd_parts` 0x229ae8 parked in `work/theseus-runs/deferred/`), 3 busy-wait loops never to rewrite (0x102240, 0x3660a8, 0x20c910). **Paused** until the 3D bug is found (decided Nov 23: rewrites reproduce the recompiler's behaviour, so they can't fix it).
- **PS2Recomp** (`references/PS2Recomp`, branch `sims-bustin-out-fixes`, latest `b5161a1`): local fixes for sign-test branches (64-bit), CVT.W.S (truncate/saturate), texture cache, scripted pad, sceMpegIsEnd, FindAddress, etc. `ps2x_tests` 457/457.

## Next steps (agreed Nov 23: get to a clean baseline, then rethink the approach)

1. ~~Incremental re-gate~~ done Nov 23: all 63 rewrites whose original changed PASS (`work/theseus-runs/regate-s64/gate.log` + b12-16 logs).
2. `_fpadd_parts` copied to `tools/theseus/rewrites/` and gated: PASS-ABI, 42/42 branches, but **16 mutant survivors** (rules gaps: read32->read16 on class/sign/exp fields, read64/write64 width, `<` -> `<=` boundaries; log `work/theseus-runs/deferred/gate-s64.log`). Ledger row: tier B, `gate-fail`. Fix the rules, then `ship.py`, rebuild, user's visible session.
3. **3D investigation** (CAS is the first real-time 3D, see above), cheapest first:
   - **Bisect the pipeline at the EE -> VU1 boundary:** are the VIF1 packets (matrices, vertices) the EE builds in CAS right? If yes, the bug is in VIF unpack / VU1 microprogram / GS; if no, in EE code. Tools: `SIMS_DMA_TRACE`, `SIMS_GIF_DUMP`, and PCSX2 (PINE reads EE RAM; compare the source buffers of the VIF1 DMA chain at the same CAS screen).
   - The game barely uses VU0 macro mode (~30 COP2 moves in the main output, no COP2 arithmetic). libgcc soft-float doubles are hot before *and* in CAS (`Fp_PackD`, `Fp_UnpackD`); HLE with host doubles remains an option (correctness bisection + speed), but CAS-only code has little 64-bit arithmetic.
   - Per-instruction edge-value tests of the recompiler's semantics vs PCSX2 (bit 31 set, nonzero upper halves, shifts >= 32, MMI). Fixes the oracle problem: the gate compares against the recompiler, so recompiler bugs pass it.
   - Ground truth: PCSX2 via PINE (MCP `pcsx2-pine`) at the same CAS screen, diff the camera/view matrix in EE RAM (start from `FUN_002a3288`'s callees 0x2B1BB8/0x2B1C40/0x2B1CE0 and whatever fills VU1 matrices each frame).
4. **Agreed Nov 23 (user priorities: a readable reference + accuracy "exactly as originally designed"):** (a) **class map from vtables** (started, see below), (b) **Unicorn as an independent reference** in the diff-test harness (original machine code in Unicorn vs recompiled C++ vs rewrite, so recompiler bugs can't pass the gate), (c) then rewrites class by class with real types. New Theseus batches wait for (b).
   - **Class recovery:** `python3 tools/classes/vtables.py` (under 1 s; reports in `out/classes/classes.{tsv,md}`). Old GCC 2.9x ABI: 8-byte entries `{int16 delta, int16 index, fn}`, zero header entry, methods from vt+8, vptr anywhere in the object (+0, +0x10, +0x44 most common). Found from constructor/destructor stores (symbolic `this` / `operator new(N)` tracking): **464 classes, 3,451 virtual methods, 264 constructors, 155 base links, 210 object sizes**; calls with a0 = this+k are members, not bases. Biggest family: root `0x3df898` (vptr +0x44, 68 classes). Known gaps: containers that construct members inline show several vptr offsets (e.g. `0x3abfe0`); the common root `0x3f08a0` (3 methods, ctor always inlined).
   - **Names:** no universal GetClassName method (only 5 classes return a string: `Object Manager`, `Action Queue Manager`, `Server Responder`, the app (`PS2 Sims Bustin Out Build 2.10.12.19-1`) and the engine (`EOR Engine v2.0 built ... Dec 19 2003`)).
   - **Idea, not done: source-file (translation unit) boundaries.** Header strings are emitted once per .cpp that includes them (`EStorable, EResource, EFontCharacter, ..., ../../../engine/e_texture.h` appears ~230 times in .rodata, in include order). String literals are private to their .cpp and .text/.rodata follow the same link order, so the .rodata block a function references gives its source file; the include list in each block tells which headers (classes) that file uses.
   - Unicorn: needs the C library (`sudo pacman -S unicorn`, extra 2.1.4, has headers). Plan: bind.cpp's `theseus_rewrite` -> a Unicorn runner over the same RAM image (uc_mem_map_ptr), R4000 CPU model (no R5900 model in QEMU/Unicorn: 128-bit regs, MMI, 3-operand mult and the PS2 FPU need our own reference); leaf integer functions first.
5. Then discuss: byte-matching decompilation as a gate (identify the compiler first: GCC-based, gp-relative small data; candidates Sony ee-gcc 2.96 / SN ProDG ee-gcc 2.95.x; no `.comment` section in the ELF).

## Build and run (from `/mnt/HDD/git_stuff/my_git/TheSimsBustinOut-Recomp`, never via the `/home/ol-randals/HDD` symlink: CMake keys objects by absolute path and would rebuild all ~11k files)

```sh
# configure once / build (~20 min full, all cores; the macro header is in every generated file)
cmake -S config/probe -B build/probe -DCMAKE_BUILD_TYPE=Release
cmake --build build/probe --target sims_boot_probe -j 12
# run (saves in runtime-data/mc0, mc1)
build/probe/sims_boot_probe work/sims-bustin-out/disc-files/SLES_520.48 <secs> [--visible]
# scripted headless run to Create-A-Sim (~900 s; expect exactly 6 error|unhandled|abort lines: known IOP import warnings)
SIMS_PAD_SCRIPT="300:cross,600:cross,1000:start,1400:start,1800:start,2400:start,2800:start,3200:down,3300:cross,3700:cross,5000:cross,5400:cross,5800:cross,6200:cross,6600:cross,7000:cross" \
  SIMS_FRAME_DUMP_DIR=work/frames-X build/probe/sims_boot_probe work/sims-bustin-out/disc-files/SLES_520.48 900 > work/X.log 2>&1
python3 tools/frame_sheet.py work/frames-X [cols] [thumb_w]     # contact sheet -> <dir>/sheet.png
```

**Regenerate after every recompiler change** (the main output once went 16 days stale, Nov 6 -> 22):

```sh
cmake --build work/sims-bustin-out/recomp-build --target ps2_recomp -j 12
cp -a work/sims-bustin-out/functions-only-output{,.before-X}; cp -a work/sims-bustin-out/supplemental-output{,.before-X}
mkdir -p /tmp/opencode && ln -s /mnt/HDD/git_stuff/my_git/TheSimsBustinOut-Recomp/work/sims-bustin-out /tmp/opencode/sims-bustin-out  # recovered.toml still uses this path; gone after reboot
work/sims-bustin-out/recomp-build/ps2xRecomp/ps2_recomp work/sims-bustin-out/recovered.toml > work/sims-bustin-out/recompile-X.log 2>&1
work/sims-bustin-out/recomp-build/ps2xRecomp/ps2_recomp config/supplemental.toml > work/sims-bustin-out/recompile-supplemental-X.log 2>&1
python3 tools/prepare_supplemental.py work/sims-bustin-out/supplemental-output --main-output work/sims-bustin-out/functions-only-output  # always --main-output (widens the table to 0x39f098)
diff -rq work/sims-bustin-out/functions-only-output{.before-X,}   # what changed
python3 tools/theseus/regate_changed.py --old-suffix .before-X [--runtime-rev <old PS2Recomp rev>]  # rewrites to re-gate
```

- PS2Recomp tests: build in `/tmp/opencode/sims-bustin-out/test-build` (`cmake --build . --target ps2x_tests -j 12`), **run from `references/PS2Recomp`** (one test reads `instructions.h` relative to the cwd).
- Native bindings: `config/native-bindings.csv` is the source of truth -> `python3 tools/prepare_native_bindings.py config/native-bindings.csv work/sims-bustin-out/native-bindings.cpp`. Missing switch targets: `tools/find_missing_targets.py`; fixes go in `config/supplemental-map.csv`.
- **Which code runs on which screen:** `SIMS_CALL_COUNTS="<poll>:<file>,...,end:<file>"` writes cumulative per-function call counts at the game's <poll>-th pad read (same counter as `SIMS_PAD_SCRIPT`) and at exit; `python3 tools/callcounts_diff.py <earlier> <later> [--all] [--tsv f]` lists functions new in (or called during) a phase with FPU/COP2/64-bit-op counts. Map frames to polls with `SIMS_PAD_LOG_POLLS=100` + `SIMS_FRAME_DUMP_DIR`. Route landmarks (Nov 23 run): main menu ~poll 3000-3200, CAS name entry ~3900, CAS ~5500.
- **Rebuild cost:** a change in `ps2xRuntime/include` (e.g. `ps2_runtime.h`, `ps2_runtime_macros.h`) recompiles all ~9,000 generated files (~20 min); a runtime `.cpp` change recompiles only itself (~3 min incl. link). Keep runtime changes in `.cpp` files where possible (declare cross-file hooks locally, as `ps2CallCountsAtPoll` does).
- Env vars: `SIMS_PAD_SCRIPT="<poll>:<btn+btn>[:<polls>],..."` (counts the game's pad reads), `SIMS_PAD_LOG_POLLS=<n>`, `SIMS_FRAME_DUMP_DIR` (every 61st frame as PPM), `SIMS_GS_TEXCACHE=0|verify`. Speed metric: last `[probe] EE cycle=` and `[frame:upload] idx=` after a fixed 180 s headless run.
- Pad: SDL2 GameController (Steam Controller works), keyboard fallback. A=X, B=O, X=□, Y=△. Don't use `SIMS_WINDOW_MONITOR` (focus breaks under XWayland).
- Profiling: `perf record -e cycles:u -F 999` (no call stacks at `perf_event_paranoid=2`; use gdb `break`/`bt` for callers).

## Theseus workflow (tools/theseus)

- Per batch: `batch_pool.py`, `batch_packet.py --next 30 B`, write `rewrites/<Name>_<addr>.cpp` + `.rules`, quick `difftest.py <name> --cases 20000`, then `gate.py <names> -j 12 --batch N` (200k-case diff test + branch coverage + caller check + mutants; ~10 min for 30). Logs in `work/theseus-runs/bNN/`.
- Ship: `ship.py` (identical copies checked byte-for-byte against the ELF), rebuild, run; `ship.py --confirm <names called in a visible session>` (`--all` for everything; an empty list is refused). The window close prints the `[theseus]` call counts.
- What the gate compares: only the 64 KB window at 0x01000000 and the stack from sp (0x01EFF000) up after every case, so anything a function *writes* must sit there; v0 low 32 bits + callee-saved regs (`ret64` for doubles / long long). The harness links only the original itself (calls are events), so re-gate only rewrites whose original changed (`regate_changed.py`).
- Rules pitfalls (details in the Nov 19-22 devlogs): give pointers/fields values with different upper halves (0x01008xxx/0x01018xxx, 0x10000-style) or width mutants survive; null-page rules for null tests; pin `a0 fixed` when fixed structures could overlap; scratch registers a caller's next callee reads -> reproduce and mark `// ...; gate: equivalent`; rules values are 32-bit; `oneof` up to 64 values. Self-recursive functions can't be diff-tested (the harness doesn't run the `jal` to itself).
- Progress map: `progress.py` (pre-commit hook regenerates it when the ledger is staged; `git config core.hooksPath tools/git-hooks`).

## Known facts worth keeping in mind

- Ruled out for the 3D bug: packed XYZ2/XYZF2 ADC bit, fog, FPU overflow (the runtime clamps like the PS2: `ps2_fadd` etc.), CVT.W.S, 32-bit sign branches, VU1 constant stores. VU1 FTOI truncates/saturates; VU1 runs with FE_TOWARDZERO. **Not done:** the EE FPU still rounds to nearest on the host (PS2: toward zero; GS rasterizing shares the thread, so switching needs care).
- The scooter intro (a movie) shows a flat grey-white ground: either how the movie looks or an MPEG decode/colour issue in our sceMpeg HLE; compare with PCSX2 sometime (not a 3D-pipeline question).
- Regeneration drops the 13 hand-traced debug files (intended); register_functions.cpp is compiled at -O0.
- Function map: recovered via Ghidra MCP (9,024 functions; some ends approximated) + supplemental map (11,192 in all).

## Data and artifacts (outside Git)

- ISO: `inputs/Sims, The - Bustin' Out (Europe) (En,Sv,No,Da,Fi).iso`; extracted disc `work/sims-bustin-out/disc-files/` (ELF SHA-256 `ca01f80b4dd960990329a30a588ef53e7a67ea111eaccff58dcdad81549f656a`).
- Generated code `work/sims-bustin-out/{functions-only-output,supplemental-output}` (+ `.before-s64` backups); recompiler build `work/sims-bustin-out/recomp-build`; inventory `out/theseus/functions.jsonl`.

## Objective and constraints

- Target the Europe PS2 release, `SLES_520.48`.
- Use only the user's legally owned disc. Never add the ISO, extracted disc files, generated game-derived source, or build products to Git.
- GitLab is primary remote; GitHub is secondary.
- Ghidra Emotion Engine Reloaded is installed locally.

## Ghidra

Ghidra currently has **Fable II** open; switch to the Sims project before using the Ghidra MCP (endpoint used for Sims: `http://127.0.0.1:18081/mcp`). Before switching targets, record Fable II's exact project/program and endpoint so it can be restored; don't guess them.

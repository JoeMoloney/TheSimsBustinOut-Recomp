# Sims Bustin' Out Recomp — Handoff

Last updated: 2026-11-25. **Keep this file short** (target ~10 KB): current state, next steps, reference.
Session history goes to `docs/devlog/` (one file per date range; the full pre-Nov-23 handoff is archived there verbatim).

## Current state

- **Boots and plays** into Create-A-Sim and the house (static recompilation with PS2Recomp, HLE for pad/mc/mpeg). Slow (~5-8x below real time; the software GS renderer is ~72% of the game thread).
- **3D FIXED (Nov 23):** after the FPU fixes (rebuild + scripted run, `work/frames-fpu/sheet.png`), Create-A-Sim draws correctly (room, Sim, parrot, window). Before: camera-inside-geometry wedges and a flat yellow sky. Most likely cause: **`sqrt.s` read fs instead of ft** (every sqrt computed sqrt(f0); 65 functions, incl. the CAS camera/float code `FUN_002b5708`); not bisected against the other four FPU fixes. **Nov 25: confirmed in the game world** (visible session: CAS and the club/house render normally, `work/frames-house2/sheet.png`; log has only the 6 known IOP warnings).
- **Theseus** (hand rewrites diff-tested against the originals): ledger 683 rows, **470 shipped, 213 gated**; 41 tier-B left, 3 deferred, 3 busy-wait loops never to rewrite. **Paused** until the 3D bug is found.
- **PS2Recomp** (`references/PS2Recomp`, branch `sims-bustin-out-fixes`): local fixes for sign-test branches (64-bit), CVT.W.S, the Nov 23 FPU fixes (sqrt operand, compares, max/min, ...), movz/movn, texture cache, scripted pad, sceMpegIsEnd, FindAddress, etc. `ps2x_tests` 460/460.

## Next steps (re-planned Nov 25)

**User goals (Nov 25):** run natively on PC and make it our own: higher resolutions, widescreen, 60 fps, mods. Readable code for the *whole* game is not the bottleneck for these (cf. Zelda64Recomp / Unleashed Recompiled: recomp + modern renderer + targeted patches). Full-game Theseus is "the long inefficient road" (470 rewrites = ~1% of 759k instructions); it is parked.

1. **GPU renderer (next).** The software GS backend is ~72% of the game thread (5-8x too slow). The runtime already splits the GS into a frontend (`ps2xRuntime/src/lib/gs/gs_frontend.cpp`) and a backend interface (`gs_cpu_backend.cpp`: `Submit(GSPrimitiveBatch)`, `UploadImage`, `LoadClut`, `Present`, transfers, `SnapshotVram`). Plan: a quick-and-dirty GPU backend beside it (keep VRAM/transfers on the CPU side, decode textures via the existing texture cache, draw triangles/sprites with GL, present), then grow it. Rough first-picture estimate ~6-10 h. Alternative to study: PCSX2's GS hardware renderer (GPL; PS2Recomp is GPL too, verify versions) as a later replacement. Upscaling comes with it.
2. **Widescreen + 60 fps:** targeted reverse engineering only (camera projection/aspect; frame timing; the game is PAL 50 Hz).
3. **Mod hooks** on top of the existing function-replacement-by-address mechanism (how Theseus rewrites plug in); content mods need the `.arc` formats.
4. **Readable code only for systems we want to mod**, via an automated pipeline (est. 10-15 h): record real inputs at function entry during play -> replay against Unicorn (original) and the rewrite; Ghidra decompile + class-map types + the Xbox build's version -> LLM to typed C++ in EOR naming. Removes the hand `.rules` step.
5. Small items: `SetExitKey(KEY_NULL)` after `InitWindow` in `ps2_runtime.cpp` (raylib exits on Esc; a Steam Controller in desktop mode sends Esc for Start). Pending re-gate of the 55 rewrites (43 shipped) whose original changed in the FPU regeneration (~20 min): `gate.py $(cat work/regate-fpu.names) -j 12 --batch regate-fpu` (header changes were FPU-only and no ledger original uses an FPU macro). `_fpadd_parts` 16 mutant survivors. Exact PS2 FPU rounding measurement (`unicorn_check.py --all-fpu --tag fpu-exact`).
- Nov 23 Unicorn/FPU details, the earlier class-by-class plan (steps A-D) and the byte-matching idea (compiler not identified; deprioritised): `docs/devlog/2026-11-23-unicorn-fpu.md` and git history of this file.

**Xbox build (Nov 25):** the user's Xbox copy is `inputs/TheSimsBustinOut_Xbox.xiso.iso`; `tools/xdvdfs_extract.py` lists/extracts it (`work/xbox-disc/default.xbe`, strings in `work/xbox-strings.txt`). No symbols, no RTTI; MSVC build `c:\eor\src2\games\sims\obj\sims_xbox_devsystem_release\Sims.exe`. Engine "EOR": `c:\eor\src2\engine\e_*.cpp/h`, classes `EFile`, `EStream`, `EResourceManager`, `EGraphics`; 26 `Class::Method` strings. `system.cnf` on the disc is a developer debug config (unlock_all_items, debug_interactions, display_fps, cam limits). Useful as a second (x86, cleaner decompile) view of the same game code.

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
- Rules pitfalls (pointer upper halves, null pages, `a0 fixed`, scratch registers, 32-bit values): see the Nov 19-22 devlogs. Self-recursive functions can't be gate-tested (the Unicorn check handles them).
- Progress map: `progress.py` (pre-commit hook regenerates it when the ledger is staged; `git config core.hooksPath tools/git-hooks`).

## Known facts worth keeping in mind

- Ruled out for the 3D bug: packed XYZ2/XYZF2 ADC bit, fog, FPU overflow (the runtime clamps like the PS2: `ps2_fadd` etc.), CVT.W.S, 32-bit sign branches, VU1 constant stores. VU1 FTOI truncates/saturates; VU1 runs with FE_TOWARDZERO. **Not done:** the EE FPU still rounds to nearest on the host (the PS2 truncates; bit-exact reference in `tools/theseus/harness/ps2_fpu_ref.h`). VU vector ops (`ps2_vfinish`) still check only the result for denormals, as the scalar ops did before Nov 23 (VU1 is in the 3D path).
- The scooter intro (a movie) shows a flat grey-white ground: that is how the movie looks (white backdrop; the user knows the original), not an MPEG bug.
- Regeneration drops the 13 hand-traced debug files (intended); register_functions.cpp is compiled at -O0.
- Function map: recovered via Ghidra MCP (9,024 functions; some ends approximated) + supplemental map (11,192 in all).

## Data and artifacts (outside Git)

- ISO: `inputs/Sims, The - Bustin' Out (Europe) (En,Sv,No,Da,Fi).iso`; extracted disc `work/sims-bustin-out/disc-files/` (ELF SHA-256 `ca01f80b4dd960990329a30a588ef53e7a67ea111eaccff58dcdad81549f656a`).
- Generated code `work/sims-bustin-out/{functions-only-output,supplemental-output}` (+ `.before-fpu` backups); recompiler build `work/sims-bustin-out/recomp-build`; inventory `out/theseus/functions.jsonl`.

## Objective and constraints

- Target the Europe PS2 release, `SLES_520.48`.
- Use only the user's legally owned disc. Never add the ISO, extracted disc files, generated game-derived source, or build products to Git.
- GitLab is primary remote; GitHub is secondary.
- Ghidra Emotion Engine Reloaded is installed locally.

## Ghidra

Ghidra currently has **Fable II** open; switch to the Sims project before using the Ghidra MCP (endpoint used for Sims: `http://127.0.0.1:18081/mcp`). Before switching targets, record Fable II's exact project/program and endpoint so it can be restored; don't guess them.

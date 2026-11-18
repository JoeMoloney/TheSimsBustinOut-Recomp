#!/usr/bin/env python3
"""Inventory every recompiled guest function (Theseus step 1).

Usage (from the repo root):
    python3 tools/theseus/inventory.py            # writes out/theseus/functions.jsonl + summary.json
    python3 tools/theseus/inventory.py --pilot    # also prints pilot candidates

Parses PS2Recomp's output, one .cpp per function (work/sims-bustin-out/functions-only-output and
supplemental-output). Each guest instruction is emitted under a comment such as
    // 0x2c85e0: 0x27bdffc0  addiu       $sp, $sp, -0x40
and every control transfer that leaves the function goes through
    runtime->dispatchGuestBranch(rdram, ctx, <target>, <source>, <fallthrough>, GuestBranchKind::<kind>, ...)
with kind DirectCall (jal), IndirectCall (jalr), DirectJump / IndirectJump (j / jr to elsewhere) or
Return (jr $ra). A plain `jr $ra` is just `ctx->pc = $ra; return;`: its Return dispatch is only
compiled with PS2X_STRICT_RETURN_DIAGNOSTICS, so `returns` counts those diagnostic sites.

Per function (one JSON object per line):
    name, start, end, bytes, insns           address range and instruction count
    file                                     generated .cpp, relative to the repo
    calls, indirect_calls, jumps, indirect_jumps, returns
    syscalls, breaks, traps, special_mem     runtime calls other than dispatch
    loops                                    eeCheckpointDue() checks = backward branches
    fpu, mmi, cop2                           instruction classes used (COP1, MMI, VU0 macro)
    leaf                                     no calls/jumps out, no syscall/break/trap
    hand_edited                              contains getenv/cout/cerr added after generation (not
                                             recompiler output: regenerate before diff-testing)
    callers                                  functions with a direct call to this one
    src, src_guess                           assert evidence (direct) and filled-in source file
"""
import collections
import glob
import json
import os
import re
import sys

import elfscan

OUT = os.path.join(elfscan.REPO, "out/theseus")
GEN_DIRS = [os.path.join(elfscan.WORK, "functions-only-output"),
            os.path.join(elfscan.WORK, "supplemental-output")]

RE_HEADER_NAME = re.compile(r"^// Function: (\S+)", re.M)
RE_HEADER_ADDR = re.compile(r"^// Address: 0x([0-9a-fA-F]+) - 0x([0-9a-fA-F]+)", re.M)
RE_INSN = re.compile(r"^\s*// 0x([0-9a-f]+): 0x[0-9a-f]+\s+([a-z0-9.]+)", re.M)
RE_DISPATCH = re.compile(r"dispatchGuestBranch\(rdram, ctx, ([^,]+), [^,]+, [^,]+, "
                         r"PS2Runtime::GuestBranchKind::(\w+)")
RE_HAND = re.compile(r"std::getenv|std::cout|std::cerr")
RE_MMI = re.compile(r"^(p(?!ref$)[a-z0-9]+|qfsrv|mfsa|mtsa|mtsab|mtsah|lq|sq)$")
COP1_MOVES = {"lwc1", "swc1", "mtc1", "mfc1", "ctc1", "cfc1"}
RE_COP2 = re.compile(r"^(v[a-z0-9]+|qmfc2|qmtc2|lqc2|sqc2|cfc2|ctc2)$")
SAVE_RESTORE = re.compile(r"^\s*// 0x[0-9a-f]+: 0x[0-9a-f]+\s+(lq|sq)\s+\$(s[0-8]|fp|ra|gp), .*\(\$sp\)", re.M)


def parse(path):
    text = open(path).read()
    name = RE_HEADER_NAME.search(text)
    addr = RE_HEADER_ADDR.search(text)
    if not (name and addr):
        return None
    start, end = int(addr.group(1), 16), int(addr.group(2), 16)
    mnems = [m.group(2) for m in RE_INSN.finditer(text)]
    kinds = collections.Counter()
    callees = set()
    for m in RE_DISPATCH.finditer(text):
        kinds[m.group(2)] += 1
        if m.group(2) == "DirectCall":
            callees.add(int(m.group(1).rstrip("uU"), 0))
    # lq/sq of callee-saved registers to the stack is just the prologue/epilogue, not MMI work.
    mmi_saves = len(SAVE_RESTORE.findall(text))
    mmi = sum(1 for x in mnems if RE_MMI.match(x)) - mmi_saves
    rec = {
        "name": name.group(1), "start": start, "end": end, "bytes": end - start, "insns": len(mnems),
        "file": os.path.relpath(path, elfscan.REPO),
        "calls": kinds["DirectCall"], "indirect_calls": kinds["IndirectCall"],
        "jumps": kinds["DirectJump"], "indirect_jumps": kinds["IndirectJump"], "returns": kinds["Return"],
        "syscalls": text.count("runtime->handleSyscall"), "breaks": text.count("runtime->handleBreak"),
        "traps": text.count("runtime->handleTrap"),
        "special_mem": len(re.findall(r"runtime->(?:Load|Store)\d", text)),
        "loops": text.count("runtime->eeCheckpointDue"),
        "fpu": sum(1 for x in mnems if x.endswith(".s") or x.endswith(".w") or x in COP1_MOVES),
        "mmi": max(0, mmi), "cop2": sum(1 for x in mnems if RE_COP2.match(x)),
        "hand_edited": bool(RE_HAND.search(text)),
        "callees": sorted(callees),
    }
    rec["leaf"] = not (rec["calls"] or rec["indirect_calls"] or rec["jumps"] or rec["indirect_jumps"]
                       or rec["syscalls"] or rec["breaks"] or rec["traps"])
    return rec


def main():
    recs = {}
    for d in GEN_DIRS:
        for path in sorted(glob.glob(os.path.join(d, "*.cpp"))):
            r = parse(path)
            if r and r["start"] not in recs:  # functions-only-output wins on duplicates
                recs[r["start"]] = r

    callers = collections.defaultdict(list)
    for r in recs.values():
        for c in r["callees"]:
            callers[c].append(r["start"])
    funcs = elfscan.load_functions()
    direct, filled = elfscan.fill_source_files(len(funcs), elfscan.source_refs(funcs))
    src = {a: (direct[k], filled[k]) for k, (a, _) in enumerate(funcs)}

    os.makedirs(OUT, exist_ok=True)
    rows = []
    for a in sorted(recs):
        r = recs[a]
        r["callers"] = sorted(callers.get(a, []))
        r["src"], r["src_guess"] = src.get(a, (None, None))
        r["start"], r["end"] = f"0x{r['start']:06x}", f"0x{r['end']:06x}"
        r["callees"] = [f"0x{c:06x}" for c in r["callees"]]
        r["callers"] = [f"0x{c:06x}" for c in r["callers"]]
        rows.append(r)
    with open(os.path.join(OUT, "functions.jsonl"), "w") as f:
        for r in rows:
            f.write(json.dumps(r, separators=(",", ":")) + "\n")

    n = len(rows)
    summary = {
        "functions": n,
        "leaf": sum(r["leaf"] for r in rows),
        "leaf_integer_only": sum(r["leaf"] and not (r["fpu"] or r["mmi"] or r["cop2"]) for r in rows),
        "with_loops": sum(r["loops"] > 0 for r in rows),
        "uses_fpu": sum(r["fpu"] > 0 for r in rows),
        "uses_mmi": sum(r["mmi"] > 0 for r in rows),
        "uses_cop2": sum(r["cop2"] > 0 for r in rows),
        "syscall_or_break": sum((r["syscalls"] or r["breaks"] or r["traps"]) > 0 for r in rows),
        "hand_edited": [r["name"] for r in rows if r["hand_edited"]],
        "with_source_file": sum(r["src_guess"] is not None for r in rows),
        "instructions": sum(r["insns"] for r in rows),
        "not_in_function_map": sum(int(r["start"], 16) not in src for r in rows),
    }
    with open(os.path.join(OUT, "summary.json"), "w") as f:
        json.dump(summary, f, indent=2)
    for k, v in summary.items():
        print(f"{k:22s} {len(v) if isinstance(v, list) else v:>8,}")
    print(f"-> {os.path.relpath(OUT, elfscan.REPO)}/functions.jsonl, summary.json")

    if "--pilot" in sys.argv:
        print("\nPilot candidates: integer-only leaves, 6..150 instructions, not hand-edited, with a source file")
        for r in rows:
            if (r["leaf"] and not (r["fpu"] or r["mmi"] or r["cop2"] or r["hand_edited"] or r["special_mem"])
                    and 6 <= r["insns"] <= 150 and r["src_guess"]):
                print(f"  {r['name']:28s} {r['insns']:4d} insns  loops={r['loops']}  callers={len(r['callers']):3d}  "
                      f"{r['src_guess']}{'' if r['src'] else ' (neighbour)'}")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Compare two SIMS_CALL_COUNTS snapshots: which functions are called between them, and which of
those were never called before? (The snapshots are cumulative, so later minus earlier = calls in
between.)

Usage (from the repo root):
    python3 tools/callcounts_diff.py <earlier.txt> <later.txt> [--all] [--top N] [--tsv out.tsv]

By default lists only functions NEW in the later phase (0 calls up to <earlier>); --all lists every
function called in between. Each row: address, calls in the phase, instructions, FPU / COP2 / MMI
instruction counts from the Theseus inventory, the number of 64-bit integer instructions (d*
arithmetic and shifts: where both recompiler bugs so far lived) counted from the
generated code's disassembly comments, and the Theseus ledger status if rewritten.
Needs out/theseus/functions.jsonl (tools/theseus/inventory.py).
"""
import argparse
import csv
import json
import os
import re

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INVENTORY = os.path.join(REPO, "out/theseus/functions.jsonl")
LEDGER = os.path.join(REPO, "tools/theseus/batch/ledger.tsv")
# 64-bit integer ops of the R5900 (MIPS III doubleword) that matter for 64-bit values.
OPS64 = {"daddu", "daddiu", "dadd", "daddi", "dsubu", "dsub", "dsll", "dsrl", "dsra", "dsll32",
         "dsrl32", "dsra32", "dsllv", "dsrlv", "dsrav", "dmult", "dmultu", "ddiv", "ddivu"}
# (ld/sd/lq/sq left out: mostly plain struct copies, they would drown the arithmetic.)
DISASM = re.compile(r"^\s*// 0x[0-9a-f]+: 0x[0-9a-f]+\s+([a-z0-9.]+)(.*)$", re.M)
# GCC writes a register move as `daddu rd, rs, $zero`: not 64-bit arithmetic.
MOVE = re.compile(r"\$zero(\s|$)")


def load(path):
    counts = {}
    with open(path) as f:
        for line in f:
            a, c = line.split()
            counts[int(a, 16)] = int(c)
    return counts


def ops64(path):
    try:
        with open(path) as f:
            return sum(1 for m in DISASM.finditer(f.read())
                       if m.group(1) in OPS64 and not (m.group(1) == "daddu" and MOVE.search(m.group(2))))
    except OSError:
        return -1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("earlier")
    ap.add_argument("later")
    ap.add_argument("--all", action="store_true", help="every function called in between, not only new ones")
    ap.add_argument("--top", type=int, default=60)
    ap.add_argument("--tsv", help="write every row to this file")
    a = ap.parse_args()

    before, after = load(a.earlier), load(a.later)
    inv = {}
    with open(INVENTORY) as f:
        for line in f:
            r = json.loads(line)
            inv[int(r["start"], 16)] = r
    status = {}
    if os.path.exists(LEDGER):
        with open(LEDGER) as f:
            for r in csv.DictReader(f, delimiter="\t"):
                status[int(r["name"].rsplit("_", 1)[1], 16)] = r["status"]

    rows = []
    for addr, c in after.items():
        delta = c - before.get(addr, 0)
        if delta <= 0 or (not a.all and before.get(addr, 0) > 0):
            continue
        r = inv.get(addr, {})
        rows.append({"addr": f"0x{addr:06x}", "calls": delta, "name": r.get("name", "?"),
                     "insns": r.get("insns", 0), "fpu": r.get("fpu", 0), "cop2": r.get("cop2", 0),
                     "mmi": r.get("mmi", 0),
                     "ops64": ops64(os.path.join(REPO, r["file"])) if "file" in r else -1,
                     "theseus": status.get(addr, "")})
    rows.sort(key=lambda r: -r["calls"])
    cols = ["addr", "calls", "insns", "fpu", "cop2", "mmi", "ops64", "theseus", "name"]
    if a.tsv:
        with open(a.tsv, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=cols, delimiter="\t", lineterminator="\n")
            w.writeheader()
            w.writerows(rows)
    kind = "called in between" if a.all else "NEW in the later phase (never called before)"
    print(f"{len(rows)} functions {kind}; earlier snapshot: {len(before)} functions, later: {len(after)}")
    print(f"{'addr':10s} {'calls':>10s} {'insns':>6s} {'fpu':>4s} {'cop2':>4s} {'mmi':>4s} {'ops64':>5s}  theseus   name")
    for r in rows[:a.top]:
        print(f"{r['addr']:10s} {r['calls']:10d} {r['insns']:6d} {r['fpu']:4d} {r['cop2']:4d} {r['mmi']:4d} "
              f"{r['ops64']:5d}  {r['theseus']:9s} {r['name']}")
    with_fpu = sum(1 for r in rows if r["fpu"])
    with64 = sum(1 for r in rows if r["ops64"] > 0)
    print(f"summary: {with_fpu} use the FPU, {with64} use 64-bit integer ops, "
          f"{sum(1 for r in rows if r['cop2'])} use COP2")


if __name__ == "__main__":
    main()

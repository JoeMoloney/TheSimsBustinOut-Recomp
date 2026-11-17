#!/usr/bin/env python3
"""Find direct control-flow targets that recompiled code cannot reach.

Loads function ranges from one or more PS2Recomp map CSVs (name,start,end,size), then
scans every direct JAL/J/branch in the ELF's text (llvm-objdump, MIPS little endian):

  call-not-entry   JAL target is not a function start
  jump-not-entry   J/branch leaves its function and lands mid-function (not a start)
  uncovered        target lies in no generated function at all

Each of these becomes a runtime '[guest-branch:missing-target]' stop when executed.
Typical cause: a recovery pass split one function into fragments (see the 0x340b80 fix).

Usage: find_missing_targets.py <elf> <map.csv> [<map.csv> ...]
"""
import bisect
import csv
import re
import subprocess
import sys
from collections import defaultdict

BRANCH_OPS = {
    "beq", "bne", "beql", "bnel", "beqz", "bnez", "beqzl", "bnezl", "blez", "bgtz", "blezl",
    "bgtzl", "bltz", "bgez", "bltzl", "bgezl", "bltzal", "bgezal", "bc1t", "bc1f", "bc1tl",
    "bc1fl", "b", "bal", "j",
}


def load_ranges(paths):
    ranges = []
    for path in paths:
        with open(path) as f:
            for row in csv.DictReader(f):
                start, end = int(row["start"], 16), int(row["end"], 16)
                if end > start:
                    ranges.append((start, end, row["name"]))
    ranges.sort()
    return ranges


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    elf, maps = sys.argv[1], sys.argv[2:]
    ranges = load_ranges(maps)
    starts = [r[0] for r in ranges]
    entry_set = set(starts)

    def containing(addr):
        i = bisect.bisect_right(starts, addr) - 1
        # ranges can overlap slightly; scan back a little for a range that covers addr
        for j in range(i, max(-1, i - 4), -1):
            if ranges[j][0] <= addr < ranges[j][1]:
                return ranges[j]
        return None

    dis = subprocess.run(
        ["llvm-objdump", "-d", "--triple=mipsel", "--mcpu=mips3", "--no-show-raw-insn", elf],
        capture_output=True, text=True, check=True).stdout
    line_re = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s+(.*)$")
    tgt_re = re.compile(r"0x([0-9a-f]+)\s*<")
    findings = defaultdict(list)  # (kind, target) -> [source pcs]
    for line in dis.splitlines():
        m = line_re.match(line)
        if not m:
            continue
        pc, op, rest = int(m.group(1), 16), m.group(2), m.group(3)
        if op != "jal" and op not in BRANCH_OPS:
            continue
        t = tgt_re.search(rest)
        if not t:
            continue
        target = int(t.group(1), 16)
        src = containing(pc)
        if src is None:
            continue  # source code itself is not generated (dead/data); skip
        dst = containing(target)
        if op == "jal":
            if target not in entry_set:
                findings[("uncovered" if dst is None else "call-not-entry", target)].append(pc)
        elif not (src[0] <= target < src[1]):
            if target not in entry_set:
                findings[("uncovered" if dst is None else "jump-not-entry", target)].append(pc)

    for (kind, target), pcs in sorted(findings.items(), key=lambda kv: (kv[0][0], kv[0][1])):
        dst = containing(target)
        where = f"inside {dst[2]} [0x{dst[0]:x},0x{dst[1]:x})" if dst else "no function"
        srcs = ", ".join(f"0x{p:x}" for p in pcs[:4]) + (" ..." if len(pcs) > 4 else "")
        print(f"{kind:15s} target=0x{target:x} {where} from {srcs}")
    print(f"# {len(findings)} distinct problem targets", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())

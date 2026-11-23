#!/usr/bin/env python3
"""Which Theseus rewrites need re-gating after a regeneration? Prints their names, one per line.

Usage (from the repo root):
    python3 tools/theseus/regate_changed.py --old-suffix .before-s64 [--runtime-rev REV] [--batch N ...]
    python3 tools/theseus/gate.py $(python3 tools/theseus/regate_changed.py --old-suffix .before-s64) -j 12 --batch regate

Why this is enough: the diff-test harness links only the rewrite's *own* original (calls, syscalls and
hardware accesses are recorded as events, not executed), and identical copies are checked byte-for-byte
against the ELF by ship.py. So a rewrite's gate result can change only if
  (a) its original's generated .cpp changed (compared with the backup copy of the output dir), or
  (b) a runtime header the harness compiles changed (macro semantics, e.g. the CVT.W.S fix).
(a) is checked here. For (b) pass --runtime-rev: if any file under ps2xRuntime/include in
references/PS2Recomp differs from REV, every rewrite is listed (a header change can touch any of them).

Back up before regenerating:  cp -a work/sims-bustin-out/functions-only-output{,.before-X}  (same for
supplemental-output). The summary goes to stderr, so $(...) gets only names.
"""
import argparse
import csv
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
LEDGER = os.path.join(HERE, "batch/ledger.tsv")
INVENTORY = os.path.join(REPO, "out/theseus/functions.jsonl")
PS2RECOMP = os.path.join(REPO, "references/PS2Recomp")


def address_of(name):
    return int(name.rsplit("_", 1)[1], 16)


def old_path(path, suffix):
    """work/.../functions-only-output/X.cpp -> work/.../functions-only-output<suffix>/X.cpp"""
    d, f = os.path.split(path)
    return os.path.join(d + suffix, f)


def same(a, b):
    if not (os.path.exists(a) and os.path.exists(b)):
        return False
    with open(a, "rb") as fa, open(b, "rb") as fb:
        return fa.read() == fb.read()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--old-suffix", required=True, help="suffix of the backup output dirs, e.g. .before-s64")
    ap.add_argument("--runtime-rev", help="PS2Recomp revision the old output was generated with")
    ap.add_argument("--batch", nargs="*", help="only rows of these ledger batches")
    a = ap.parse_args()

    files = {}
    with open(INVENTORY) as f:
        for line in f:
            r = json.loads(line)
            files[int(r["start"], 16)] = r["file"]
    with open(LEDGER) as f:
        rows = [r for r in csv.DictReader(f, delimiter="\t")
                if r["status"] != "rejected" and (not a.batch or r["batch"] in a.batch)]

    header_change = []
    if a.runtime_rev:
        p = subprocess.run(["git", "diff", "--name-only", a.runtime_rev, "--", "ps2xRuntime/include"],
                           cwd=PS2RECOMP, capture_output=True, text=True, check=True)
        header_change = p.stdout.split()

    changed, missing = [], []
    for r in rows:
        rel = files.get(address_of(r["name"]))
        if rel is None:
            missing.append(r["name"])
            continue
        new = os.path.join(REPO, rel)
        if header_change or not same(new, old_path(new, a.old_suffix)):
            changed.append(r)

    for r in changed:
        print(r["name"])
    print(f"regate_changed: {len(changed)} of {len(rows)} rewrites need re-gating "
          f"({sum(r['status'] == 'shipped' for r in changed)} shipped)", file=sys.stderr)
    if header_change:
        print(f"regate_changed: runtime headers changed since {a.runtime_rev}, so all are listed: "
              + ", ".join(header_change), file=sys.stderr)
    if missing:
        print(f"regate_changed: WARNING {len(missing)} ledger rows have no inventory entry "
              f"(run inventory.py): {' '.join(missing[:10])}", file=sys.stderr)


if __name__ == "__main__":
    main()

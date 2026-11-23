#!/usr/bin/env python3
"""Check the recompiler against Unicorn on many functions: `difftest.py <name> --unicorn` for each,
in parallel, then one summary line per function and the totals.

Usage (from the repo root):
    python3 tools/theseus/unicorn_check.py [names...] [--all-ledger] [-j 12] [--cases 50000]

--all-ledger takes every ledger row whose original the Unicorn runner models (no FPU, MMI/128-bit,
COP2 or hardware accesses), so their .rules files give realistic inputs.

Verdicts (from the harness output):
  MATCH      every register (scratch ones too) and all compared memory equal in every case
  ABI-ONLY   the ABI compare passes, but some non-ABI register differs: still a recompiler
             difference, worth a look
  MISMATCH   the ABI compare fails: the recompiled code and the machine code disagree
  ERROR      build/run problem
Writes out/theseus/unicorn/summary.tsv.
"""
import argparse
import concurrent.futures
import csv
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
LEDGER = os.path.join(HERE, "batch/ledger.tsv")
INVENTORY = os.path.join(REPO, "out/theseus/functions.jsonl")
OUT = os.path.join(REPO, "out/theseus/unicorn")
UNMODELLED = ("fpu", "mmi", "cop2", "special_mem")


def check(name, cases):
    p = subprocess.run([sys.executable, os.path.join(HERE, "difftest.py"), name, "--unicorn", "--cases", str(cases)],
                       capture_output=True, text=True, cwd=REPO)
    text = p.stdout + p.stderr
    m = re.search(r"^differing_fields:(.*)$", text, re.M)
    fields = m.group(1).strip() if m else "?"
    skipped = re.search(r"^skipped_cases=(\d+)", text, re.M)
    result = re.search(r"^RESULT (\S+)", text, re.M)
    if not result:
        verdict = "ERROR"
    elif result.group(1) == "FAIL":
        verdict = "MISMATCH"
    else:
        verdict = "MATCH" if not fields else "ABI-ONLY"
    first = [l for l in text.splitlines() if l.startswith(("first ABI failure", "unicorn:", "  register", "  mem "))][:4]
    return {"name": name, "verdict": verdict, "differing": fields, "skipped": skipped.group(1) if skipped else "0",
            "detail": " | ".join(first)}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("names", nargs="*")
    ap.add_argument("--all-ledger", action="store_true")
    ap.add_argument("-j", type=int, default=12)
    ap.add_argument("--cases", type=int, default=50000)
    a = ap.parse_args()
    names = list(a.names)
    if a.all_ledger:
        inv = {}
        with open(INVENTORY) as f:
            for line in f:
                r = json.loads(line)
                inv[int(r["start"], 16)] = r
        with open(LEDGER) as f:
            for r in csv.DictReader(f, delimiter="\t"):
                o = inv.get(int(r["name"].rsplit("_", 1)[1], 16))
                if o and not any(o.get(k) for k in UNMODELLED) and not o.get("hand_edited"):
                    names.append(r["name"])
    print(f"{len(names)} functions, {a.cases} cases each", flush=True)
    with concurrent.futures.ThreadPoolExecutor(a.j) as pool:
        rows = list(pool.map(lambda n: check(n, a.cases), names))
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "summary.tsv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    for r in rows:
        if r["verdict"] != "MATCH":
            print(f"{r['verdict']:9s} {r['name']:44s} differing: {r['differing'] or '-'}  {r['detail']}")
    counts = {}
    for r in rows:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
    print("totals:", ", ".join(f"{k} {v}" for k, v in sorted(counts.items())))
    print(f"summary: {os.path.relpath(OUT, REPO)}/summary.tsv")


if __name__ == "__main__":
    main()

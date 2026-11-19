#!/usr/bin/env python3
"""Build the Theseus batch queue from the inventory (port of Fable II's batch_pool.py).

Usage (from the repo root, after tools/theseus/inventory.py):
    python3 tools/theseus/batch_pool.py                 # write the queue, print counts
    python3 tools/theseus/batch_pool.py --top 50 [A B]  # also list the next 50 (optionally only tiers A/B)

Reads out/theseus/functions.jsonl and tools/theseus/batch/ledger.tsv; writes
out/theseus/batch/queue.jsonl (not tracked): every pool function not yet in the ledger, ranked.

Pool: integer-only leaves (no calls or jumps out, no syscall/break/trap, no FPU/MMI/COP2, no
hardware addresses, no switch tables), not hand-edited, outside the Sony SDK range.

Identical copies: functions whose bytes are identical are one queue entry (the lowest address
is the representative). One rewrite serves them all: list the others in the rewrite's
`// theseus: copies 0x..., 0x...` line (see elfscan.py).

Tiers:
    A  has direct callers, no stores
    B  has direct callers, stores to memory
    D  no direct callers (called through vtables/pointers: callers can't be checked)
    T  trivial: 3 instructions or fewer (`jr $ra` + one instruction, empty functions)
Inside a tier: most call sites (all copies together) first, then smallest first.
"""
import argparse
import collections
import csv
import json
import os
import re
import sys

import elfscan

HERE = os.path.dirname(os.path.abspath(__file__))
INVENTORY = os.path.join(elfscan.REPO, "out/theseus/functions.jsonl")
LEDGER = os.path.join(HERE, "batch/ledger.tsv")
OUT = os.path.join(elfscan.REPO, "out/theseus/batch")
# Sony SDK (libkernel/libgraph/libpad/libmc/libmpeg/...) is linked from 0x376000 up; replacing it
# gives nothing (it is replaced by HLE where it matters) and its assembly breaks ABI rules.
SDK_START = 0x376000
STORE = re.compile(r"\b(sb|sh|sw|sd|sq|swl|swr|sdl|sdr)\b")


def in_pool(f):
    return (f["leaf"] and not (f["fpu"] or f["mmi"] or f["cop2"] or f["hand_edited"] or f["special_mem"]
                               or f["indirect_jumps"]) and int(f["start"], 16) < SDK_START)


def has_stores(f):
    text = open(os.path.join(elfscan.REPO, f["file"])).read()
    return any(STORE.search(m) for m in re.findall(r"^\s*// 0x[0-9a-f]+: 0x[0-9a-f]+\s+([a-z0-9.]+)", text, re.M))


def tier_of(f, n_callers, stores):
    if f["insns"] <= 3:
        return "T"
    if n_callers == 0:
        return "D"
    return "B" if stores else "A"


def ledger_addresses():
    """Addresses already handled: ledger rows plus the copies their rewrites list."""
    done = set()
    if os.path.exists(LEDGER):
        for r in csv.DictReader(open(LEDGER), delimiter="\t"):
            m = re.search(r"_([0-9a-fA-F]{6,8})$", r["name"])
            if m:
                done.add(int(m.group(1), 16))
                done.update(elfscan.rewrite_copies(r["name"]))
    return done


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--top", type=int, default=0, help="list the next N queued entries")
    ap.add_argument("tiers", nargs="*", default=[], help="tiers to list with --top (default all)")
    a = ap.parse_args()
    if not os.path.exists(INVENTORY):
        sys.exit("run tools/theseus/inventory.py first")
    funcs = [json.loads(l) for l in open(INVENTORY)]
    pool = [f for f in funcs if in_pool(f)]
    seg = elfscan.segment()
    done = ledger_addresses()

    groups = collections.defaultdict(list)
    for f in pool:
        groups[elfscan.body(seg, int(f["start"], 16), f["insns"])].append(f)
    queue = []
    for members in groups.values():
        members.sort(key=lambda f: int(f["start"], 16))
        addrs = [int(f["start"], 16) for f in members]
        if any(x in done for x in addrs):
            continue
        rep = members[0]
        n_callers = sum(len(f["callers"]) for f in members)
        stores = has_stores(rep)
        queue.append({
            "name": rep["name"], "start": rep["start"], "tier": tier_of(rep, n_callers, stores),
            "insns": rep["insns"], "n_callers": n_callers, "loops": rep["loops"], "stores": stores,
            "copies": [f["start"] for f in members[1:]], "src": rep["src_guess"], "file": rep["file"],
        })
    queue.sort(key=lambda q: ("ABDT".index(q["tier"]), -q["n_callers"], q["insns"], q["start"]))
    for i, q in enumerate(queue):
        q["rank"] = i
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "queue.jsonl"), "w") as fh:
        for q in queue:
            fh.write(json.dumps(q) + "\n")

    count = collections.Counter(q["tier"] for q in queue)
    funcs_in = collections.Counter()
    for q in queue:
        funcs_in[q["tier"]] += 1 + len(q["copies"])
    print(f"pool {len(pool)} functions, {len(done)} already in the ledger; queued {len(queue)} entries "
          f"({sum(funcs_in.values())} functions incl. identical copies)")
    print("by tier (entries / functions): " + "  ".join(f"{t} {count[t]}/{funcs_in[t]}" for t in "ABDT"))
    if a.top:
        sel = [q for q in queue if not a.tiers or q["tier"] in a.tiers][:a.top]
        for q in sel:
            extra = f" +{len(q['copies'])} copies" if q["copies"] else ""
            loop = " loop" if q["loops"] else ""
            print(f"  {q['tier']} {q['start']} {q['insns']:3d} insns {q['n_callers']:4d} call sites{loop}{extra}"
                  + (f"  [{q['src']}]" if q["src"] else ""))


if __name__ == "__main__":
    main()

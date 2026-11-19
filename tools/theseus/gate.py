#!/usr/bin/env python3
"""Theseus gate: every check a rewrite must pass before it can ship, in one command.

Usage (from the repo root):
    python3 tools/theseus/gate.py <rewrite>... [-j N] [--batch NAME] [--no-ledger]

Per rewrite (tools/theseus/rewrites/<rewrite>.cpp):
  1. difftest.py at 200,000 cases: PASS-ABI or PASS-STRICT, and every conditional branch of the
     original went both ways (coverage; a direction no valid input reaches is listed in the
     .rules file as `uncovered 0xADDR taken|not <reason>`).
  2. caller_usage.py: no caller reads a scratch register the rewrite leaves different.
  3. Rewrite mutants: small changes to the rewrite (an integer literal +1, a comparison flipped,
     a read/write width changed, a store dropped). Each must FAIL a 20,000-case diff test,
     which shows the random inputs reach every line. A survivor fails the gate; if the change
     really is equivalent, mark the line `// gate: equivalent <token>: <reason>` (skips only the
     mutants whose replacement contains <token>, e.g. read16).
Then a row in tools/theseus/batch/ledger.tsv (same columns as Fable II's ledger): status
`gated` or `gate-fail` (rows already `reviewed`, `shipped` or `rejected` keep their status).
Writes out/theseus/gate/<rewrite>/gate.json.
"""
import argparse
import concurrent.futures
import csv
import datetime
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
REWRITES = os.path.join(HERE, "rewrites")
LEDGER = os.path.join(HERE, "batch/ledger.tsv")
OUT = os.path.join(REPO, "out/theseus/gate")
COLUMNS = ["name", "tier", "status", "gate", "kill_rate", "batch", "note"]
KEEP_STATUS = {"reviewed", "shipped", "rejected"}
MUTANT_CASES = 20000

# (pattern, replacement) pairs applied one occurrence at a time to code lines of the rewrite.
SWAPS = [(r"!=", "=="), (r"==", "!="), (r"<=", "<"), (r"(?<![<-])<(?![<=])", "<="),
         (r">=", ">"), (r"(?<![>-])>(?![>=])", ">="),
         (r"\bread8\(", "read16("), (r"\bread16\(", "read8("), (r"\bread16\(", "read32("),
         (r"\bread32\(", "read16("), (r"\bwrite8\(", "write16("), (r"\bwrite16\(", "write8("),
         (r"\bwrite16\(", "write32("), (r"\bwrite32\(", "write16("),
         (r"\bread64\(", "read32("), (r"\bwrite64\(", "write32("), (r">>", "<<"), (r"<<", ">>"),
         (r"\bsread8\(", "read8("), (r"\bsread16\(", "read16(")]


def run(cmd, env=None):
    p = subprocess.run(cmd, capture_output=True, text=True, cwd=REPO, env=env)
    return p.returncode, p.stdout + p.stderr


def code_lines(src):
    """Indexes of lines a mutant may change: code inside functions/namespaces, not comments,
    includes or marked-equivalent lines."""
    out = []
    for i, line in enumerate(src):
        stripped = line.strip()
        if not stripped or stripped.startswith(("//", "#", "namespace", "{", "}", "void ")):
            continue
        out.append(i)
    return out


def mutants(path):
    src = open(path).read().split("\n")
    result = []
    for i in code_lines(src):
        line = src[i]
        code, _, comment = line.partition("//")
        # `static_cast<T>(...)`: its angle brackets are not comparisons (masked for matching).
        masked = re.sub(r"\w+_cast<[^<>]*>", lambda m: "_" * len(m.group(0)), code)
        for pat, rep in SWAPS:
            for m in re.finditer(pat, masked):
                new = code[:m.start()] + rep + code[m.end():]
                result.append((i + 1, f"{m.group(0)} -> {rep}", new + ("//" + comment if comment else "")))
        for m in re.finditer(r"\b(0x[0-9A-Fa-f]+|\d+)u?\b", code):
            value = int(m.group(1), 0)
            lit = hex(value + 1) if m.group(1).lower().startswith("0x") else str(value + 1)
            new = code[:m.start(1)] + lit + code[m.end(1):]
            result.append((i + 1, f"{m.group(1)} -> {lit}", new + ("//" + comment if comment else "")))
        if re.search(r"\bg\.write(8|16|32)\(", code):
            result.append((i + 1, "drop store", "// (dropped) " + line.strip()))
    # `// gate: equivalent <token>: <reason>` skips the mutants on that line whose replacement
    # contains <token> (e.g. read16); a bare `// gate: equivalent` skips the whole line.
    kept = []
    for lineno, what, new in result:
        marker = re.search(r"gate: equivalent(?: (\S+?):?(?:\s|$))?", src[lineno - 1])
        if marker and (marker.group(1) is None or marker.group(1) in what.split("->")[-1]):
            continue
        kept.append((lineno, what, new))
    texts = []
    for lineno, what, new in kept:
        copy = list(src)
        copy[lineno - 1] = new
        texts.append((lineno, what, "\n".join(copy)))
    return texts


def gate(name, batch):
    out = os.path.join(OUT, name)
    os.makedirs(out, exist_ok=True)
    rec = {"name": name, "date": datetime.date.today().isoformat()}

    code, text = run([sys.executable, os.path.join(HERE, "difftest.py"), name])
    m = re.search(r"^RESULT (\S+)", text, re.M)
    rec["difftest"] = m.group(1) if m else "ERROR"
    rec["difftest_ok"] = code == 0
    if not rec["difftest_ok"]:
        rec["detail"] = text[-2000:]
    rec["coverage_gaps"] = re.findall(r"^  coverage gap: (.+)$", text, re.M)
    cov = re.search(r"^COVERAGE \S+ \((\S+) branch", text, re.M)
    rec["coverage"] = cov.group(1) if cov else "?"

    code, text = run([sys.executable, os.path.join(HERE, "caller_usage.py"), name])
    rec["callers_ok"] = code == 0
    rec["callers"] = text.strip()
    sites = re.search(r"(\d+) call sites", text)
    rec["call_sites"] = int(sites.group(1)) if sites else 0

    killed, survivors, errors = 0, [], []
    env = dict(os.environ, DIFFTEST_STOP="1")
    for k, (lineno, what, text_src) in enumerate(mutants(os.path.join(REWRITES, name + ".cpp"))):
        path = os.path.join(out, f"mutant{k}.cpp")
        with open(path, "w") as f:
            f.write(text_src)
        code, text = run([sys.executable, os.path.join(HERE, "difftest.py"), name, "--rewrite", path,
                          "--variant", "mutant", "--cases", str(MUTANT_CASES)], env)
        if "compile failed" in text:
            errors.append(f"line {lineno}: {what} (does not compile)")
        elif code != 0:
            killed += 1
        else:
            survivors.append(f"line {lineno}: {what}")
    total = killed + len(survivors)
    rec["mutants"] = total
    rec["killed"] = killed
    rec["survivors"] = survivors
    rec["not_compiling"] = errors
    rec["kill_rate"] = round(killed / total, 3) if total else None
    rec["passed"] = rec["difftest_ok"] and rec["callers_ok"] and not survivors and not rec["coverage_gaps"]
    with open(os.path.join(out, "gate.json"), "w") as f:
        json.dump(rec, f, indent=2)
    rec["batch"] = batch
    return rec


def update_ledger(recs):
    rows = []
    if os.path.exists(LEDGER):
        with open(LEDGER) as f:
            rows = list(csv.DictReader(f, delimiter="\t"))
    by_name = {r["name"]: r for r in rows}
    for rec in recs:
        row = by_name.get(rec["name"])
        if row is None:
            row = {c: "" for c in COLUMNS}
            row.update(name=rec["name"], tier="C", batch=rec["batch"])
            rows.append(row)
        if row["status"] not in KEEP_STATUS:
            row["status"] = "gated" if rec["passed"] else "gate-fail"
        row["gate"] = rec["difftest"]
        row["kill_rate"] = "" if rec["kill_rate"] is None else f"{rec['kill_rate']:.3f}"
        row["note"] = (f"gate.py {rec['date']}: {rec['difftest']}; mutants {rec['killed']}/{rec['mutants']}; "
                       f"callers {'ok' if rec['callers_ok'] else 'READ'} ({rec['call_sites']} sites); "
                       f"branches {rec['coverage']}")
    with open(LEDGER, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS, delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("names", nargs="+")
    ap.add_argument("-j", type=int, default=1, help="rewrites gated in parallel")
    ap.add_argument("--batch", default="pilot")
    ap.add_argument("--no-ledger", action="store_true")
    a = ap.parse_args()
    names = [os.path.splitext(os.path.basename(n))[0] for n in a.names]
    with concurrent.futures.ThreadPoolExecutor(a.j) as pool:
        recs = list(pool.map(lambda n: gate(n, a.batch), names))
    for r in recs:
        mark = "PASS" if r["passed"] else "FAIL"
        print(f"{mark}  {r['name']:38s} {r['difftest']:12s} callers {'ok' if r['callers_ok'] else 'READ':4s} "
              f"({r['call_sites']} sites)  mutants {r['killed']}/{r['mutants']}  branches {r['coverage']}"
              + "".join(f"\n        coverage gap {g}" for g in r["coverage_gaps"])
              + "".join(f"\n        survivor {s}" for s in r["survivors"])
              + "".join(f"\n        not compiling {s}" for s in r["not_compiling"]))
    if not a.no_ledger:
        update_ledger(recs)
        print(f"ledger: {os.path.relpath(LEDGER, REPO)}")
    sys.exit(0 if all(r["passed"] for r in recs) else 1)


if __name__ == "__main__":
    main()

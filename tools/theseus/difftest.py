#!/usr/bin/env python3
"""Diff-test a Theseus rewrite against the recompiled original (200,000 random states).

Usage (from the repo root, after tools/theseus/inventory.py):
    python3 tools/theseus/difftest.py <rewrite> [--cases N] [--seed S] [--timeout SECONDS]
                                                [--rewrite FILE]

<rewrite> names tools/theseus/rewrites/<rewrite>.cpp, which defines
    void <rewrite>(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
The name ends in the original's address (e.g. bstring_AddRef_002c85c8); the original is found
in out/theseus/functions.jsonl. Optional tools/theseus/rewrites/<rewrite>.rules sets inputs
(see the header of harness/difftest_harness.cpp). --rewrite tests another file under the same
name, e.g. a deliberately broken copy.

Builds with the game's own compile flags (from build/probe/compile_commands.json) and writes
out/theseus/difftest/<rewrite>/{difftest, build.log, result.txt}. Exit status 0 = PASS.
"""
import argparse
import hashlib
import json
import os
import re
import shlex
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
REWRITES = os.path.join(HERE, "rewrites")
HARNESS = os.path.join(HERE, "harness")
INVENTORY = os.path.join(REPO, "out/theseus/functions.jsonl")
COMPILE_DB = os.path.join(REPO, "build/probe/compile_commands.json")
ELF = os.path.join(REPO, "work/sims-bustin-out/disc-files/SLES_520.48")
OUT = os.path.join(REPO, "out/theseus/difftest")

REG_NAMES = {f"a{i}": 4 + i for i in range(8)}
REG_NAMES.update({"v0": 2, "v1": 3, "gp": 28, "sp": 29, "fp": 30, "ra": 31})
REG_NAMES.update({f"s{i}": 16 + i for i in range(8)})
KINDS = {"any": "kAny", "ptr": "kPtr", "ptr_or_null": "kPtrOrNull", "int": "kInt", "u32": "kU32",
         "len": "kLen", "slen": "kSLen", "fixed": "kFixed", "oneof": "kOneOf",
         "str": "kStr8", "wstr16": "kStr16", "wstr32": "kStr32", "aptr": "kAPtr", "alen": "kALen"}


def die(msg):
    print(f"difftest: {msg}", file=sys.stderr)
    sys.exit(2)


def reg_number(token):
    if token in REG_NAMES:
        return REG_NAMES[token]
    m = re.fullmatch(r"r(\d+)", token)
    if m and 1 <= int(m.group(1)) <= 31:
        return int(m.group(1))
    die(f"unknown register {token!r} in rules")


def kind_and_value(words, where):
    """('kLen', 256, [alts]) from e.g. ['len', '256']."""
    if not words or words[0] not in KINDS:
        die(f"{where}: expected one of {', '.join(KINDS)}")
    kind, rest = words[0], words[1:]
    if kind == "oneof":
        if not 1 <= len(rest) <= 8:
            die(f"{where}: oneof takes 1..8 values")
        return KINDS[kind], 0, [int(v, 0) for v in rest]
    needs_value = kind in ("len", "slen", "fixed", "str", "wstr16", "wstr32", "alen")
    if needs_value != bool(rest):
        die(f"{where}: {kind} {'needs a value' if needs_value else 'takes no value'}")
    return KINDS[kind], int(rest[0], 0) if rest else 0, []


def rules_inc(path):
    """Turn a .rules file into the C++ tables the harness includes."""
    args, mems, datas = [], [], []
    for n, line in enumerate(open(path), 1):
        words = line.split("#", 1)[0].split()
        if not words:
            continue
        where = f"{os.path.basename(path)}:{n}"
        if words[0] == "mem32":
            if len(words) < 3:
                die(f"{where}: mem32 <addr|@reg+off|*addr+off> <kind> [value]")
            target, base, deref = words[1], 0, 0
            if m := re.fullmatch(r"@(\w+)\+(\w+)", target):
                base, addr = reg_number(m.group(1)), int(m.group(2), 0)
            elif m := re.fullmatch(r"\*(\w+)\+(\w+)", target):
                deref, addr = int(m.group(1), 0), int(m.group(2), 0)
            else:
                addr = int(target, 0)
            kind, value, alts = kind_and_value(words[2:], where)
            mems.append((addr, kind, value, base, deref, alts))
        elif words[0] == "data":
            if len(words) != 3 or not re.fullmatch(r"([0-9a-fA-F]{2})+", words[2]):
                die(f"{where}: data <addr> <hex bytes>")
            datas.append((int(words[1], 0), words[2]))
        else:
            kind, value, alts = kind_and_value(words[1:], where)
            args.append((reg_number(words[0]), kind, value, alts))

    def alts_init(alts):
        return "{" + ", ".join(f"0x{a:x}u" for a in alts) + "}"

    out = ["const ArgRule kRules[] = {"]
    out += [f"    {{{r}, {k}, 0x{v:x}u, {alts_init(a)}, {len(a)}}}," for r, k, v, a in args] or ["    {0, kAny, 0, {}, 0},"]
    out += ["};", "const MemRule kMemRules[] = {"]
    out += [f"    {{0x{a:x}u, {k}, 0x{v:x}u, {b}, 0x{d:x}u, {alts_init(al)}, {len(al)}}},"
            for a, k, v, b, d, al in mems] or ["    {0, kAny, 0, 0, 0, {}, 0},"]
    out += ["};", "const DataRule kDataRules[] = {"]
    out += [f'    {{0x{a:x}u, "{h}"}},' for a, h in datas] or ['    {0u, ""},']
    out += ["};"]
    return "\n".join(out) + "\n"


def game_flags():
    """Compile flags of a generated game file, minus PCH, output and per-function logging."""
    db = json.load(open(COMPILE_DB))
    entry = next(e for e in db if "/functions-only-output/" in e["file"])
    words = shlex.split(entry["command"])[1:]
    flags, skip = [], False
    for w in words:
        if skip:
            skip = False
            continue
        if w in ("-o", "-c", "-include"):
            skip = True
            continue
        if w == entry["file"] or w == "-Winvalid-pch" or w.startswith("-DPS2_FUNCTION_LOG_TRACKER"):
            continue
        flags.append(w)
    return flags


def find_original(address):
    for line in open(INVENTORY):
        r = json.loads(line)
        if int(r["start"], 16) == address:
            return r
    die(f"no function at 0x{address:x} in {INVENTORY} (run inventory.py)")


BIND = """// Generated by difftest.py: forwards the harness's two entry points to the functions under test.
#include <cstdint>
struct R5900Context;
class PS2Runtime;
void {orig}(uint8_t *, R5900Context *, PS2Runtime *);
void {rewrite}(uint8_t *, R5900Context *, PS2Runtime *);
void theseus_original(uint8_t *r, R5900Context *c, PS2Runtime *p) {{ {orig}(r, c, p); }}
void theseus_rewrite(uint8_t *r, R5900Context *c, PS2Runtime *p) {{ {rewrite}(r, c, p); }}
"""


def stamp(cmd, inputs):
    """Fingerprint of a compile: the command line plus the contents of every input. An object
    is reused only if its .stamp matches, so switching the source (e.g. --rewrite) or editing
    any input always rebuilds; file times are not trusted."""
    h = hashlib.sha1(" ".join(cmd).encode())
    for path in inputs:
        h.update(path.encode())
        with open(path, "rb") as f:
            h.update(f.read())
    return h.hexdigest()


def write_if_changed(path, text):
    if not os.path.exists(path) or open(path).read() != text:
        with open(path, "w") as f:
            f.write(text)


def build(name, rewrite_cpp, orig, rules, outdir, variant=None):
    """Compile (cached) and link the harness. The heavy part is parsing the runtime headers, so
    every object is reused until one of its inputs changes; editing a rewrite rebuilds only it."""
    os.makedirs(outdir, exist_ok=True)
    shared = os.path.join(OUT, "_shared")
    os.makedirs(shared, exist_ok=True)
    src = os.path.join(REPO, orig["file"])
    symbol = re.search(r"^void (\w+)\(uint8_t\* rdram", open(src).read(), re.M).group(1)
    flags = game_flags() + [f"-I{HERE}", f"-I{HARNESS}"]
    flags_file = os.path.join(shared, "flags.txt")  # a flag change rebuilds everything
    write_if_changed(flags_file, " ".join(flags) + "\n")
    rules_inc_path = os.path.join(outdir, "difftest_rules.inc")
    write_if_changed(rules_inc_path, rules_inc(rules) if rules else "")
    bind = os.path.join(outdir, "bind.cpp")
    write_if_changed(bind, BIND.format(orig=symbol, rewrite=name))
    own_headers = [os.path.join(HERE, "theseus.h"), os.path.join(HARNESS, "runtime_stub.h"), flags_file]
    harness_src = os.path.join(HARNESS, "difftest_harness.cpp")
    units = [  # (object, source, extra flags, inputs that force a rebuild)
        (os.path.join(outdir, "harness.o"), harness_src,
         [f"-I{outdir}"] + (["-DHAS_RULES"] if rules else []), [harness_src, rules_inc_path] + own_headers),
        (os.path.join(shared, "runtime_stub.o"), os.path.join(HARNESS, "runtime_stub.cpp"), [],
         [os.path.join(HARNESS, "runtime_stub.cpp")] + own_headers),
        (os.path.join(outdir, "original.o"), src, [], [src, flags_file]),
        (os.path.join(outdir, f"rewrite-{variant}.o" if variant else "rewrite.o"), rewrite_cpp, [],
         [rewrite_cpp] + own_headers),
        (os.path.join(outdir, "bind.o"), bind, [], [bind, flags_file]),
    ]
    log = open(os.path.join(outdir, f"build-{variant}.log" if variant else "build.log"), "w")
    procs = []
    for obj, source, extra, inputs in units:
        cmd = ["c++"] + flags + extra + ["-c", source, "-o", obj]
        key = stamp(cmd, [source] + inputs)
        stamp_file = obj + ".stamp"
        if os.path.exists(obj) and os.path.exists(stamp_file) and open(stamp_file).read() == key:
            continue
        if os.path.exists(stamp_file):
            os.unlink(stamp_file)  # invalid until this compile succeeds
        log.write(" ".join(shlex.quote(c) for c in cmd) + "\n")
        log.flush()
        procs.append((obj, stamp_file, key, subprocess.Popen(cmd, stdout=log, stderr=subprocess.STDOUT)))
    failed = []
    for obj, stamp_file, key, p in procs:
        if p.wait():
            failed.append(obj)
        else:
            with open(stamp_file, "w") as f:
                f.write(key)
    if failed:
        die(f"compile failed, see {os.path.relpath(log.name, REPO)}")
    exe = os.path.join(outdir, f"difftest-{variant}" if variant else "difftest")
    link = ["c++", "-o", exe] + [u[0] for u in units]
    if subprocess.run(link, stdout=log, stderr=subprocess.STDOUT).returncode:
        die(f"link failed, see {os.path.relpath(log.name, REPO)}")
    return exe, symbol


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("name")
    ap.add_argument("--cases", type=int, default=200000)
    ap.add_argument("--seed", default="0x9E3779B97F4A7C15")
    ap.add_argument("--timeout", type=float, default=600)
    ap.add_argument("--rewrite", help="test this file instead of rewrites/<name>.cpp")
    ap.add_argument("--variant", help="build/result suffix, so mutants don't replace the main build")
    a = ap.parse_args()

    m = re.search(r"_([0-9a-fA-F]{6,8})$", a.name)
    if not m:
        die("rewrite name must end in the original's address, e.g. bstring_AddRef_002c85c8")
    orig = find_original(int(m.group(1), 16))
    if orig["hand_edited"]:
        die(f"{orig['file']} has hand-added debug code; regenerate it before diff-testing")
    rewrite_cpp = os.path.abspath(a.rewrite) if a.rewrite else os.path.join(REWRITES, a.name + ".cpp")
    rules = os.path.join(REWRITES, a.name + ".rules")
    outdir = os.path.join(OUT, a.name)
    exe, symbol = build(a.name, rewrite_cpp, orig, rules if os.path.exists(rules) else None, outdir, a.variant)
    print(f"original {symbol} ({orig['insns']} insns, {orig['file']})")
    print(f"rewrite  {os.path.relpath(rewrite_cpp, REPO)}" + (f" + {os.path.relpath(rules, REPO)}" if os.path.exists(rules) else ""))
    try:
        run = subprocess.run([exe, str(a.cases), a.seed], capture_output=True, text=True, timeout=a.timeout,
                             env=dict(os.environ, THESEUS_ELF=ELF))
        text, code = run.stdout + run.stderr, run.returncode
    except subprocess.TimeoutExpired:
        text, code = f"RESULT FAIL (timeout after {a.timeout:.0f} s)\n", 1
    with open(os.path.join(outdir, f"result-{a.variant}.txt" if a.variant else "result.txt"), "w") as f:
        f.write(text)
    print(text, end="")
    sys.exit(0 if code == 0 else 1)


if __name__ == "__main__":
    main()

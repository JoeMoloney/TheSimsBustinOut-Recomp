#!/usr/bin/env python3
"""Evidence packet + draft input rules for one queued function (port of Fable II's batch_packet.py).

Usage (from the repo root, after batch_pool.py):
    python3 tools/theseus/batch_packet.py 0x2c8e38 [0x...]     # packets for these functions
    python3 tools/theseus/batch_packet.py --next 25 [A B]       # the next 25 in the queue (tiers)

Writes out/theseus/batch/<address>/packet.md and draft.rules, and prints one summary line per
function. A packet holds what writing a rewrite needs, in one place:
  - header: size, loops, call sites, identical copies, source-file guess
  - the disassembly (from the generated file's comments), branch targets as labels
  - draft .rules (below), each line with the instruction that suggested it
  - up to 3 call sites: the caller's instructions before the `jal`, i.e. how it sets the arguments

Draft rules come from a forward pass over the instructions that tracks where each register's
value came from (argument aN, a field loaded from one, a global, a constant) without following
branches, so a rule can be wrong after a join; difftest's branch coverage and the gate's mutants
catch what a draft misses. Guesses:
  aN used as a load/store base          -> aN ptr        (compared with zero first: ptr_or_null)
  a field of aN used as a base          -> mem32 @aN+off ptr
  a global used as a base               -> mem32 0xADDR ptr
  byte/halfword loads from aN in a loop compared with zero -> aN str 64 / wstr16 64
  aN in a loop compare, not a pointer   -> aN len 64
  aN otherwise used as a value          -> u32 (int when compared/shifted as signed)
  constants aN is compared with         -> listed (an `oneof` may reach the branches better)
"""
import argparse
import json
import os
import re
import sys

import elfscan

QUEUE = os.path.join(elfscan.REPO, "out/theseus/batch/queue.jsonl")
INVENTORY = os.path.join(elfscan.REPO, "out/theseus/functions.jsonl")
OUT = os.path.join(elfscan.REPO, "out/theseus/batch")
GP = 0x3FFAF0

RE_INSN = re.compile(r"^\s*// 0x([0-9a-f]+): 0x[0-9a-f]+\s+([a-z0-9.]+)\s*(.*?)\s*(\(Delay Slot\))?$")
RE_LABEL = re.compile(r"^label_([0-9a-f]+):")
REGS = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]
LOADS = {"lb": 1, "lbu": 1, "lh": 2, "lhu": 2, "lw": 4, "lwu": 4, "ld": 8, "lq": 16,
         "lwl": 4, "lwr": 4, "ldl": 8, "ldr": 8}
STORES = {"sb": 1, "sh": 2, "sw": 4, "sd": 8, "sq": 16, "swl": 4, "swr": 4, "sdl": 8, "sdr": 8}
BRANCH2 = {"beq", "bne", "beql", "bnel"}
BRANCH1 = {"beqz", "bnez", "blez", "bgtz", "bltz", "bgez", "beqzl", "bnezl", "blezl", "bgtzl", "bltzl", "bgezl"}
MOVES = {"daddu", "addu", "or", "move"}  # with $zero as one operand: a copy
SIGNED = {"slt", "slti", "sra", "srav", "blez", "bgtz", "bltz", "bgez", "blezl", "bgtzl", "bltzl", "bgezl"}


def parse_insns(path):
    """[(addr, mnemonic, [operands], is_delay_slot)] and the set of label addresses."""
    insns, labels = [], set()
    for line in open(path):
        if m := RE_LABEL.match(line.strip()):
            labels.add(int(m.group(1), 16))
        elif m := RE_INSN.match(line):
            ops = [o.strip() for o in m.group(3).split(",")] if m.group(3) else []
            insns.append((int(m.group(1), 16), m.group(2), ops, bool(m.group(4))))
    return insns, labels


def reg(op):
    op = op.strip().lstrip("$")
    return REGS.index(op) if op in REGS else None


def imm(op):
    try:
        return int(op, 0)
    except ValueError:
        return None


def mem_operand(op):
    """'0x10($a0)' -> (0x10, 4)"""
    m = re.fullmatch(r"(-?0x[0-9a-fA-F]+|-?\d+)\(\$(\w+)\)", op.strip())
    return (int(m.group(1), 0), reg(m.group(2))) if m else (None, None)


def describe(v):
    if v is None:
        return "?"
    kind = v[0]
    if kind == "arg":
        return f"a{v[1]}"
    if kind == "field":
        return f"[{describe(v[1])}+0x{v[2]:x}]"
    if kind == "glob":
        return f"[0x{v[1]:06x}]"
    if kind == "const":
        return f"0x{v[1]:x}"
    return kind


class Draft:
    """Forward pass over a leaf's instructions; collects rule suggestions with evidence.

    Values are tagged by origin: ("arg", i), ("field", base, off), ("glob", addr), ("const", k),
    ("gp",), ("ptr+", root) = root plus an offset/index, ("idx", root) = root shifted or scaled.
    Facts are kept per root value (an argument, a field, a global)."""

    def __init__(self, insns, loops):
        self.insns, self.loops = insns, loops
        self.ptr = {}            # root -> first (addr, mnemonic) using it as a load/store base
        self.loaded = {}         # field/global value -> load mnemonic (in program order)
        self.null_checked = set()
        self.compared = {}       # root -> set of constants and/or "reg"
        self.signed = set()
        self.used = set()        # roots used as values (arithmetic, compares)
        self.stored = set()      # roots stored to memory
        self.loop_bound = set()  # roots tested by a backward branch
        self.added = []          # (root, root) pairs added together (pointer + index)
        self.scaled = set()      # roots shifted/multiplied (array indexes)
        self.zero_tests = set()  # (arg, width): characters loaded through arg tested against 0
        self.offsets = {}        # base root -> offsets accessed
        self.notes = []

    @staticmethod
    def root(v):
        return v[1] if v and v[0] in ("ptr+", "idx") else v

    def run(self):
        st = {i: None for i in range(32)}
        for i in range(8):
            st[4 + i] = ("arg", i)
        st[28] = ("gp",)
        st[0] = ("const", 0)
        chars = {}  # register -> (arg, width) of a character loaded through that argument
        for addr, mn, ops, _ in self.insns:
            if mn in LOADS or mn in STORES:
                off, b = mem_operand(ops[1]) if len(ops) > 1 else (None, None)
                if b is None:
                    continue
                base = st[b]
                self.base_use(base, off, addr, mn)
                if mn in STORES:
                    v = self.root(st[reg(ops[0])])
                    if v and v[0] in ("arg", "field", "glob"):
                        self.stored.add(v)  # copied, not used: random bytes are enough
                    continue
                d = reg(ops[0])
                val, rb = None, (base[1] if base and base[0] == "ptr+" else base)
                exact = base is not None and base[0] != "ptr+" and off is not None
                if rb and rb[0] in ("arg", "field") and exact:
                    val = ("field", rb, off)
                elif rb and rb[0] == "glob" and exact:
                    val = ("glob", rb[1] + off)
                elif rb and rb[0] == "gp" and off is not None:
                    val = ("glob", GP + off)
                elif rb and rb[0] == "const" and off is not None:
                    val = ("glob", (rb[1] + off) & 0xFFFFFFFF)
                if val and val[0] in ("field", "glob"):
                    self.loaded.setdefault(val, mn)
                width = {"lb": 1, "lbu": 1, "lh": 2, "lhu": 2, "lw": 4}.get(mn)
                chars.pop(d, None)
                if width and rb and rb[0] == "arg":
                    chars[d] = (rb[1], width)
                if mn not in ("lwl", "lwr", "ldl", "ldr") or st[d] is None:
                    st[d] = val
                continue
            if mn == "lui":
                st[reg(ops[0])] = ("const", (imm(ops[1]) or 0) << 16)
                chars.pop(reg(ops[0]), None)
                continue
            if mn in ("addiu", "daddiu", "ori"):
                d, sv, k = reg(ops[0]), st[reg(ops[1])], imm(ops[2])
                self.mark_used(sv)
                if sv and sv[0] == "const" and k is not None:
                    st[d] = ("const", (sv[1] | (k & 0xFFFF)) if mn == "ori" else (sv[1] + k) & 0xFFFFFFFF)
                elif sv and sv[0] in ("arg", "field", "glob", "ptr+") and mn != "ori":
                    st[d] = sv if sv[0] == "ptr+" else ("ptr+", sv)
                else:
                    st[d] = None
                chars.pop(d, None)
                continue
            if mn in MOVES and len(ops) == 3 and (reg(ops[2]) == 0 or reg(ops[1]) == 0):
                src = reg(ops[1]) if reg(ops[2]) == 0 else reg(ops[2])
                st[reg(ops[0])] = st[src]
                if src in chars:
                    chars[reg(ops[0])] = chars[src]
                continue
            if mn in ("addu", "daddu") and len(ops) == 3:
                a, b = st[reg(ops[1])], st[reg(ops[2])]
                ra, rb = self.root(a), self.root(b)
                if ra and rb and ra[0] != "const" and rb[0] != "const":
                    self.added.append((ra, rb))
                self.mark_used(a)
                self.mark_used(b)
                p = next((x for x in (a, b) if x and x[0] in ("arg", "field", "glob", "ptr+")), None)
                st[reg(ops[0])] = (p if p[0] == "ptr+" else ("ptr+", p)) if p else None
                chars.pop(reg(ops[0]), None)
                continue
            if mn in ("sll", "dsll", "mult", "mul") and len(ops) >= 2:
                sv = self.root(st[reg(ops[1])])
                self.mark_used(sv)
                if sv and sv[0] in ("arg", "field", "glob"):
                    self.scaled.add(sv)
                if mn in ("sll", "dsll"):
                    st[reg(ops[0])] = ("idx", sv) if sv and sv[0] in ("arg", "field", "glob") else None
                    continue
            if self.backward(ops) and (mn in BRANCH2 or mn in BRANCH1):
                for o in ops[:2 if mn in BRANCH2 else 1]:
                    v = self.root(st[reg(o)]) if reg(o) is not None else None
                    if v and v[0] in ("arg", "field", "glob"):
                        self.loop_bound.add(v)
            if mn in BRANCH2 and len(ops) >= 2:
                a, b = reg(ops[0]), reg(ops[1])
                self.compare(st[a], st[b], mn)
                self.compare(st[b], st[a], mn)
                for r, other in ((a, b), (b, a)):
                    if r in chars and st[other] == ("const", 0):
                        self.zero_tests.add(chars[r])
                continue
            if mn in BRANCH1 and ops:
                a = reg(ops[0])
                self.compare(st[a], ("const", 0), mn)
                if a in chars:
                    self.zero_tests.add(chars[a])
                continue
            if mn in ("slt", "sltu", "slti", "sltiu") and len(ops) == 3:
                other = st[reg(ops[2])] if mn in ("slt", "sltu") else ("const", imm(ops[2]) & 0xFFFFFFFF)
                self.compare(st[reg(ops[1])], other, mn)
                self.compare(other, st[reg(ops[1])], mn)
            if mn in ("andi", "xori") and len(ops) == 3:  # `andi t, x, K` then a test of t: x vs K
                sv = self.root(st[reg(ops[1])])
                if sv and sv[0] in ("arg", "field", "glob"):
                    self.compared.setdefault(sv, set()).add(imm(ops[2]) & 0xFFFF)
            # anything else: operands are used as values, destination unknown
            regs = [reg(o) for o in ops]
            for r in regs[1:]:
                if r is not None:
                    self.mark_used(st[r])
                    if mn in SIGNED:
                        v = self.root(st[r])
                        if v:
                            self.signed.add(v)
            if regs and regs[0] is not None and mn not in BRANCH2 | BRANCH1 | {"jr", "jalr", "b", "nop"}:
                st[regs[0]] = None
                chars.pop(regs[0], None)
        return self

    @staticmethod
    def backward(ops):
        """True for a branch whose target ('. + 4 + (-0x6 << 2)') is at or before itself."""
        m = re.search(r"\. \+ 4 \+ \((-?0x[0-9a-fA-F]+|-?\d+) << 2\)", ", ".join(ops))
        return bool(m) and 4 + (int(m.group(1), 0) << 2) <= 0

    def base_use(self, base, off, addr, mn):
        b = self.root(base)
        if b is None or b[0] in ("gp", "const"):
            return
        self.ptr.setdefault(b, (addr, mn))
        if off is not None and base and base[0] != "ptr+":
            self.offsets.setdefault(b, set()).add(off)

    def mark_used(self, v):
        v = self.root(v)
        if v and v[0] in ("arg", "field", "glob"):
            self.used.add(v)

    def compare(self, v, other, mn):
        v = self.root(v)
        if not v or v[0] not in ("arg", "field", "glob"):
            return
        self.used.add(v)
        if mn in SIGNED or mn in ("slt", "slti"):
            self.signed.add(v)
        if other == ("const", 0) and mn in BRANCH1 | BRANCH2:
            self.null_checked.add(v)
        if other and other[0] == "const":
            self.compared.setdefault(v, set()).add(other[1])
        else:
            self.compared.setdefault(v, set()).add("reg")

    def value_kind(self, v):
        """(kind, evidence) for a value that is not a pointer."""
        index = v in self.scaled or any(v in pair and any(p in self.ptr for p in pair if p != v)
                                        for pair in self.added)
        if v in self.loop_bound:
            return "len 64", "counts a loop (tested by a backward branch)"
        if index:
            return "len 64", "index into a pointer (shifted and/or added to one)"
        cmp = self.compared.get(v, set())
        consts = sorted(c for c in cmp if c != "reg")
        if consts and consts != [0]:  # only zero tests: u32/int give 0 and other edges often enough
            vals = []
            for c in consts + [0, 0xFFFFFFFF] + [(c + d) & 0xFFFFFFFF for c in consts for d in (1, -1)]:
                if c not in vals:
                    vals.append(c)
            return ("oneof " + " ".join(hex(x) for x in vals[:8]),
                    f"compared with {', '.join(hex(c) for c in consts)} (plus neighbours/edges; random values miss them)")
        if self.loops and "reg" in cmp:
            return "len 64", "compared with another register in a loop function: count/bound?"
        if self.loops and v[0] == "arg":
            return "len 256", "value in a function with loops: bounded so a hidden count can't run for minutes (widen if coverage says so)"
        return ("int" if v in self.signed else "u32"), ("signed compare/shift" if v in self.signed else "used as a value")

    def rules(self):
        """[(rule line, evidence)]"""
        out = []
        for i in range(8):
            a = ("arg", i)
            if a in self.ptr:
                at, mn = self.ptr[a]
                width = max((w for (x, w) in self.zero_tests if x == i), default=None)
                if width and self.loops:
                    kind = {1: "str", 2: "wstr16", 4: "wstr32"}[width]
                    out.append((f"a{i} {kind} 64", f"{8 * width}-bit characters loaded through a{i}, tested for 0 in a loop"))
                elif a in self.null_checked:
                    out.append((f"a{i} ptr_or_null", f"base of {mn} at 0x{at:06x}, compared with 0"))
                else:
                    out.append((f"a{i} ptr", f"base of {mn} at 0x{at:06x}"))
            elif a in self.used:
                kind, why = self.value_kind(a)
                out.append((f"a{i} {kind}", why))
        strings = {i for i in range(8) if any(r.startswith((f"a{i} str", f"a{i} wstr")) for r, _ in out)}
        for v in list(self.loaded) + [p for p in self.ptr if p not in self.loaded]:
            if v[0] == "field" and v[1][0] == "arg" and v[1][1] in strings:
                continue  # the string's characters: a rule would overwrite them
            if v[0] == "field" and v[1][0] == "arg":
                where = f"@a{v[1][1]}+0x{v[2]:x}"
            elif v[0] == "glob":
                where = f"0x{v[1]:06x}"
            elif v[0] == "field":
                if v in self.ptr or v in self.used:
                    self.notes.append(f"{describe(v)} is read through a pointer chain: needs a deeper rule "
                                      "(mem32 *addr+off or data), or random window bytes")
                continue
            else:
                continue
            if v in self.ptr:
                at, mn = self.ptr[v]
                null = v in self.null_checked
                out.append((f"mem32 {where} {'ptr_or_null' if null else 'ptr'}",
                            f"used as base of {mn} at 0x{at:06x}" + (", compared with 0" if null else "")))
            elif v in self.used:
                kind, why = self.value_kind(v)
                narrow = self.loaded.get(v) in ("lb", "lbu", "lh", "lhu")
                if kind in ("u32", "int") and (narrow or v[0] == "field" and v[2] % 4):
                    continue  # random window bytes already vary a narrow field; mem32 would overwrite neighbours
                out.append((f"mem32 {where} {kind}", f"{self.loaded.get(v, 'read')}: {why}"))
        for b, offs in self.offsets.items():
            big = [o for o in offs if o >= 0x4000 or o < 0]
            if big:
                self.notes.append(f"{describe(b)} accessed at offsets {', '.join(hex(o) for o in sorted(big))}: "
                                  "pointer rules point into the first 32 KB of a 64 KB window")
        return out


def caller_context(start, callers, n=3, before=6):
    """Instructions before up to n `jal` sites of `start`, from the callers' generated files."""
    files = {int(f["start"], 16): f["file"] for f in map(json.loads, open(INVENTORY))}
    want = f"jal         func_{start:X}"
    out = []
    for c in callers:
        path = files.get(int(c, 16))
        if not path:
            continue
        lines = [l.rstrip() for l in open(os.path.join(elfscan.REPO, path)) if RE_INSN.match(l)]
        for k, l in enumerate(lines):
            if l.strip().endswith(want) or re.search(rf"jal\s+func_{start:X}\b", l):
                seg = lines[max(0, k - before):k + 2]
                out.append((c, [re.sub(r"^\s*// ", "", s) for s in seg]))
                break
        if len(out) >= n:
            break
    return out


def packet(q, inv):
    start = int(q["start"], 16)
    f = inv[start]
    insns, labels = parse_insns(os.path.join(elfscan.REPO, f["file"]))
    d = Draft(insns, f["loops"]).run()
    rules = d.rules()
    outdir = os.path.join(OUT, f"{start:06x}")
    os.makedirs(outdir, exist_ok=True)
    md = [f"# 0x{start:06x} ({f['name']})", "",
          f"- tier {q['tier']}, {f['insns']} instructions, loops {f['loops']}, stores {'yes' if q['stores'] else 'no'}",
          f"- {q['n_callers']} call sites (incl. copies), callers: {', '.join(f['callers'][:8])}"
          + (" ..." if len(f["callers"]) > 8 else ""),
          f"- identical copies: {', '.join(q['copies']) or 'none'}"
          + ("  -> rewrite header `// theseus: copies " + ", ".join(q["copies"]) + "`" if q["copies"] else ""),
          f"- source file: {f['src'] or (f['src_guess'] + ' (neighbour guess)' if f['src_guess'] else 'unknown')}",
          f"- generated: {f['file']}", "", "## Disassembly", "```"]
    for addr, mn, ops, ds in insns:
        if addr in labels:
            md.append(f"L_{addr:06x}:")
        md.append(f"  {addr:06x}  {mn:8s} {', '.join(ops)}{'   # delay slot' if ds else ''}")
    md += ["```", "", "## Draft rules (check them; written to draft.rules)", "```"]
    md += [f"{r:28s} # {why}" for r, why in rules] or ["# no argument is read"]
    md += ["```"]
    if d.notes:
        md += ["", "Notes:"] + [f"- {n}" for n in d.notes]
    ctx = caller_context(start, f["callers"])
    if ctx:
        md += ["", "## Call sites (instructions before the jal)"]
        for c, seg in ctx:
            md += [f"caller {c}:", "```"] + seg + ["```"]
    with open(os.path.join(outdir, "packet.md"), "w") as fh:
        fh.write("\n".join(md) + "\n")
    with open(os.path.join(outdir, "draft.rules"), "w") as fh:
        fh.write("".join(f"{r:28s} # draft: {why}\n" for r, why in rules))
    return rules, d.notes, outdir


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("addresses", nargs="*")
    ap.add_argument("--next", type=int, default=0)
    a = ap.parse_args()
    if not os.path.exists(QUEUE):
        sys.exit("run tools/theseus/batch_pool.py first")
    queue = [json.loads(l) for l in open(QUEUE)]
    inv = {int(f["start"], 16): f for f in map(json.loads, open(INVENTORY))}
    if a.next:
        tiers = [x for x in a.addresses if x in ("A", "B", "D", "T")]
        sel = [q for q in queue if not tiers or q["tier"] in tiers][:a.next]
    else:
        by = {int(q["start"], 16): q for q in queue}
        sel = []
        for x in a.addresses:
            if int(x, 16) not in by:
                sys.exit(f"0x{int(x, 16):06x} is not in the queue (in the ledger, or not a pool function)")
            sel.append(by[int(x, 16)])
    for q in sel:
        rules, notes, outdir = packet(q, inv)
        print(f"{q['tier']} {q['start']} {q['insns']:3d} insns {q['n_callers']:4d} sites  "
              f"{'; '.join(r for r, _ in rules) or '-'}" + (f"  [{len(notes)} notes]" if notes else ""))
    print(f"packets in {os.path.relpath(OUT, elfscan.REPO)}/<address>/packet.md")


if __name__ == "__main__":
    main()

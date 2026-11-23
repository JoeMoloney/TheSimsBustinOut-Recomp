#!/usr/bin/env python3
"""Recover C++ classes from the game's vtables (static analysis of SLES_520.48, no Ghidra needed).

Usage (from the repo root):
    python3 tools/classes/vtables.py [--out out/classes]

How it works (old GCC 2.9x C++ ABI, which this game uses):
  * A vtable is an array of 8-byte entries {int16 delta, int16 index, function pointer}. Entry 0 is
    all zero (no RTTI); the virtual methods follow from vtable+8. A nonzero delta is a `this`
    adjustment (multiple inheritance).
  * The object's vtable pointer is NOT always at +0: g++ 2.x puts it where the first class with
    virtual functions declares its first one (seen: +0, +0x18, +0x5C).
  * Constructors and destructors store the vtable address into the object:
        lui rX, hi ; addiu rX, rX, lo ; sw rX, off(this)
    so every such store of an address that looks like a vtable is evidence of a class.
    A function that stores V and is itself a method of V is (almost always) V's destructor;
    one that stores V and is not in V is a constructor (or an inlined one).
  * Inheritance: a constructor/destructor of V that calls another function storing V' at the
    same offset means V derives from V' (constructors call the base constructor first; destructors
    reset the vtable and call the base destructor last).
  * Object size: `li a0, N ; jal operator_new` followed by a call to a constructor of V.

Outputs (git-ignored, derived from the game): <out>/classes.tsv (one row per vtable) and
<out>/classes.md (readable report). Function names come from the Theseus ledger when rewritten.
"""
import argparse
import collections
import csv
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, "tools/theseus"))
import elfscan  # noqa: E402

LEDGER = os.path.join(REPO, "tools/theseus/batch/ledger.tsv")
OPERATOR_NEW = 0x1025F0  # Theseus: OperatorNew_001025f0 (Heap_AllocAligned, alignment 4)
DATA_LO, DATA_HI = 0x3A4400, 0x3F9300  # .data .. .sbss (vtables live in .data/.rodata)
CALLER_SAVED = set(range(1, 16)) | {24, 25, 31}


def s16(x):
    return x - 0x10000 if x & 0x8000 else x


class Image:
    def __init__(self):
        self.seg = elfscan.segment()
        self.funcs = elfscan.load_functions()
        self.starts = {a for a, _ in self.funcs}

    def word(self, addr):
        o = addr - elfscan.SEG_VA
        if 0 <= o <= len(self.seg) - 4:
            return struct.unpack_from("<I", self.seg, o)[0]
        return None

    def vtable_methods(self, vt):
        """[(slot, delta, function)] if vt looks like a vtable (zero header, then function entries)."""
        if not (DATA_LO <= vt < DATA_HI) or vt % 8:
            return None
        if self.word(vt) != 0 or self.word(vt + 4) != 0:
            return None
        methods = []
        a = vt + 8
        while True:
            hdr, pfn = self.word(a), self.word(a + 4)
            if hdr is None or pfn not in self.starts or (hdr & 0xFFFF) != 0:
                break  # index half is 0 in every entry this compiler emits
            methods.append((len(methods) + 1, s16(hdr >> 16) if hdr >> 16 else 0, pfn))
            a += 8
        return methods or None


THIS = "this"  # symbolic register value ("this", offset): the function's incoming a0 plus offset


def scan_function(img, start, size):
    """Yield events from one function: ('store', value, where, pc) for every sw/sd of a known
    constant, where = (base, offset) if the address is a tracked pointer + offset (base "this" = the
    incoming a0, or ("new", size) = the result of operator new(size)), else None; and
    ('call', target, a0, pc) for every jal, a0 = an int constant, ("this", offset) or None.
    Register values are tracked linearly (branches are ignored), which is enough for the short
    constructor/destructor prologues this looks at."""
    regs = {0: 0, 4: (THIS, 0)}
    end = start + size

    def add(v, k):
        if isinstance(v, tuple):
            return (v[0], v[1] + k)
        return (v + k) & 0xFFFFFFFF

    def step(pc):
        w = img.word(pc)
        if w is None:
            return None
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        imm = w & 0xFFFF
        ev = None
        if op == 0x0F:  # lui
            regs[rt] = (imm << 16) & 0xFFFFFFFF
        elif op in (0x09, 0x19):  # addiu / daddiu
            if rs in regs:
                regs[rt] = add(regs[rs], s16(imm))
            else:
                regs.pop(rt, None)
        elif op == 0x0D:  # ori
            if rs in regs and isinstance(regs[rs], int):
                regs[rt] = regs[rs] | imm
            else:
                regs.pop(rt, None)
        elif op in (0x2B, 0x3F):  # sw / sd
            if rt in regs and isinstance(regs[rt], int):
                base = regs.get(rs)
                where = (base[0], base[1] + s16(imm)) if isinstance(base, tuple) else None
                ev = ("store", regs[rt], where, pc)
        elif op == 0x00:  # R-type: writes rd; `daddu rd, rs, $zero` / `addu` / `or` are moves
            rd, funct = (w >> 11) & 31, w & 0x3F
            if funct in (0x21, 0x2D, 0x25) and rt == 0 and rs in regs:
                regs[rd] = regs[rs]
            elif funct in (0x21, 0x2D, 0x25) and rs == 0 and rt in regs:
                regs[rd] = regs[rt]
            elif funct not in (0x08, 0x09, 0x0C, 0x0D, 0x18, 0x19, 0x1A, 0x1B):  # jr/jalr/syscall/break/mult/div
                regs.pop(rd, None)
            if funct == 0x09:  # jalr
                regs.pop(rd, None)
        elif 0x20 <= op <= 0x27 or op in (0x37, 0x1E, 0x0A, 0x0B, 0x0C, 0x0E, 0x08, 0x18, 0x1A, 0x1B):
            regs.pop(rt, None)  # loads, lq, slti, sltiu, andi, xori, addi, daddi, ldl, ldr
        regs[0] = 0
        return w, op, ev

    pc = start
    while pc < end:
        r = step(pc)
        if r is None:
            break
        w, op, ev = r
        if ev:
            yield ev
        if op == 0x03:  # jal: run the delay slot first (often `li a0, size` / `move a0, s0`)
            target = (pc & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            r2 = step(pc + 4)
            if r2 and r2[2]:
                yield r2[2]
            yield ("call", target, regs.get(4), pc)
            a0 = regs.get(4)
            for reg in CALLER_SAVED:
                regs.pop(reg, None)
            if target == OPERATOR_NEW and isinstance(a0, int):
                regs[2] = (("new", a0), 0)  # v0 = the new object of a0 bytes
            pc += 8
            continue
        pc += 4


def ledger_names():
    names = {}
    if os.path.exists(LEDGER):
        with open(LEDGER) as f:
            for r in csv.DictReader(f, delimiter="\t"):
                names[int(r["name"].rsplit("_", 1)[1], 16)] = r["name"].rsplit("_", 1)[0]
    return names


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=os.path.join(REPO, "out/classes"))
    a = ap.parse_args()
    img = Image()
    names = ledger_names()

    stores = collections.defaultdict(list)       # vtable -> [(function, where)]
    calls = collections.defaultdict(list)        # function -> [(target, a0, pc)]
    new_sizes = collections.defaultdict(list)    # function -> [(pc, size)] of operator new calls
    vtables = {}
    for start, size in img.funcs:
        for ev in scan_function(img, start, size):
            if ev[0] == "store":
                _, value, where, pc = ev
                if value not in vtables:
                    m = img.vtable_methods(value)
                    if m is None:
                        continue
                    vtables[value] = m
                stores[value].append((start, where))
            else:
                _, target, a0, pc = ev
                calls[start].append((target, a0, pc))
                if target == OPERATOR_NEW and isinstance(a0, int):
                    new_sizes[start].append((pc, a0))

    # Constructors / destructors per vtable: functions that store V into their own `this`.
    # The vptr offset is where they store it. Stores elsewhere (into a freshly allocated object,
    # i.e. an inlined constructor in a factory) are kept apart as `inline_in`.
    funcs_storing = collections.defaultdict(dict)  # function -> {vtable: vptr offset in this}
    for vt, st in stores.items():
        for f, where in st:
            if where is not None and where[0] == THIS:
                funcs_storing[f][vt] = where[1]
    all_methods = {fn for m in vtables.values() for _, _, fn in m}
    classes = {}
    for vt, methods in vtables.items():
        members = {f for _, _, f in methods}
        own = [(f, where[1]) for f, where in stores[vt] if where is not None and where[0] == THIS]
        new = [(f, where) for f, where in stores[vt] if where is not None and where[0] != THIS]
        # A derived class's destructor resets the vptr to its base's vtable before running the
        # base part: it stores V but is a method of another vtable -> not a constructor of V.
        ctors = sorted({f for f, _ in own if f not in all_methods})
        dtors = sorted({f for f, _ in own if f in members})
        classes[vt] = {"methods": methods, "ctors": ctors, "dtors": dtors,
                       "vptr": sorted({off for _, off in own} | {w[1] for _, w in new}),
                       "inline_in": sorted({f for f, _ in new}),
                       "bases": set(), "members": set(),
                       "sizes": {w[0][1] for _, w in new}, "name": None}

    # Inheritance vs composition. A ctor/dtor of V that calls a ctor/dtor of V' (V' != V):
    #   with a0 = this + 0  -> V' is the (primary) base class of V;
    #   with a0 = this + k  -> V' is embedded at +k (a member object, or a secondary base).
    # A ctor/dtor of V that itself stores V' into this (inlined base/member constructor) counts the
    # same way, judged by where V' goes relative to V's own vptr... too ambiguous: left out.
    for vt, c in classes.items():
        for f in c["ctors"] + c["dtors"]:
            for target, a0, _ in calls[f]:
                if not isinstance(a0, tuple):
                    continue
                for other in funcs_storing.get(target, ()):
                    oc = classes.get(other)
                    if other == vt or oc is None or target not in oc["ctors"] + oc["dtors"]:
                        continue
                    if a0[1] == 0:
                        c["bases"].add(other)
                    else:
                        c["members"].add((a0[1], other))

    # Object sizes: operator new(N) followed (in the same function) by a call to a ctor of V.
    ctor_of = collections.defaultdict(set)
    for vt, c in classes.items():
        for f in c["ctors"]:
            ctor_of[f].add(vt)
    for f, news in new_sizes.items():
        for pc, n in news:
            later = [t for t, _, cpc in calls[f] if cpc > pc and t in ctor_of]
            if later:
                for vt in ctor_of[later[0]]:
                    classes[vt]["sizes"].add(n)
    # Inlined constructors (operator new(N), then the vtable stored into the result in the same
    # function) are not used for sizes: a factory may allocate several objects.

    write_reports(a.out, img, classes, names)


def fname(names, f):
    return f"{names[f]}@{f:x}" if f in names else f"{f:x}"


def write_reports(out, img, classes, names):
    os.makedirs(out, exist_ok=True)
    rows = []
    for vt in sorted(classes):
        c = classes[vt]
        rows.append({
            "vtable": f"0x{vt:x}", "methods": len(c["methods"]),
            "vptr_offsets": " ".join(f"0x{o:x}" for o in c["vptr"]),
            "sizes": " ".join(f"0x{s:x}" for s in sorted(c["sizes"])),
            "bases": " ".join(f"0x{b:x}" for b in sorted(c["bases"])),
            "members": " ".join(f"+0x{k:x}:0x{m:x}" for k, m in sorted(c["members"])),
            "inline_in": " ".join(f"0x{f:x}" for f in c["inline_in"]),
            "ctors": " ".join(f"0x{f:x}" for f in c["ctors"]),
            "dtors": " ".join(f"0x{f:x}" for f in c["dtors"]),
            "slots": " ".join(f"{s}:0x{f:x}" + (f"(d{d})" if d else "") for s, d, f in c["methods"]),
        })
    with open(os.path.join(out, "classes.tsv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)

    method_funcs = {f for c in classes.values() for _, _, f in c["methods"]}
    ctor_funcs = {f for c in classes.values() for f in c["ctors"]}
    derived = sum(1 for c in classes.values() if c["bases"])
    sized = sum(1 for c in classes.values() if c["sizes"])
    roots = [vt for vt, c in classes.items() if not c["bases"]]
    children = collections.defaultdict(list)
    for vt, c in classes.items():
        for b in c["bases"]:
            children[b].append(vt)

    lines = ["# Classes recovered from vtables", "",
             f"- {len(classes)} vtables (classes), {len(method_funcs)} distinct virtual methods, "
             f"{len(ctor_funcs)} constructors; {len(img.funcs)} functions in the map.",
             f"- {derived} classes with a base class found, {sized} with an object size.",
             f"- vptr offsets: " + ", ".join(f"0x{o:x}: {n}" for o, n in collections.Counter(
                 o for c in classes.values() for o in c["vptr"]).most_common(8)), "",
             "## Hierarchy (largest trees first)", ""]

    def subtree_size(vt, seen=()):
        return 1 + sum(subtree_size(k, seen + (vt,)) for k in children[vt] if k not in seen)

    def show(vt, depth, seen):
        c = classes[vt]
        lines.append(f"{'  ' * depth}- `0x{vt:x}` {len(c['methods'])} methods"
                     + (f", size {', '.join(hex(s) for s in sorted(c['sizes']))}" if c["sizes"] else "")
                     + (f", vptr +{', +'.join(hex(o) for o in c['vptr'])}")
                     + (f", ctor {fname(names, c['ctors'][0])}" if c["ctors"] else ""))
        for k in sorted(children[vt], key=lambda k: -subtree_size(k)):
            if k not in seen:
                show(k, depth + 1, seen | {k})

    for vt in sorted(roots, key=lambda v: -subtree_size(v)):
        show(vt, 0, {vt})
    with open(os.path.join(out, "classes.md"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("\n".join(lines[:6]))
    print(f"reports: {os.path.relpath(out, REPO)}/classes.tsv, classes.md")


if __name__ == "__main__":
    main()

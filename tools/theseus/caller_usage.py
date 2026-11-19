#!/usr/bin/env python3
"""Check that no caller reads a scratch register a rewrite leaves different (Theseus).

Usage (from the repo root):
    python3 tools/theseus/caller_usage.py <rewrite or address> [reg ...]

Without registers, the scratch registers come from the rewrite's diff-test result
(out/theseus/difftest/<rewrite>/result.txt, `differing_fields`).

For every `jal <function>` in the game ELF, the caller's code after the call is followed
(both ways at every branch, through delay slots) until each register is overwritten or
becomes dead. A register is READ if an instruction uses it first. At a call or tail jump the
next function may read only its arguments (a0..a7); at `jr $ra` the caller's own caller may
read only v0/v1. Anything the decoder can't follow (`jr` through a register other than $ra,
an unknown instruction, a long path) counts as a read.

ABI rule (where the code can't be followed: indirect calls `jalr`, syscalls, switch jumps
`jr <reg>`): a scratch register the caller has not written since our call is dead there. GCC
treats every scratch register as clobbered by a call, so compiled code cannot read one or pass
it on without writing it first. Survey (2026-11-18) of all 42,912 calls inside known functions,
16 scratch registers each (~687,000 checks): 11 reads, all after calls that never return (the
function map runs on into the next function) or in Sony SDK assembly.

The function's address appearing as data or built with lui+addiu means it may also be called
through a pointer (callers unknown); that is reported as a warning.
Exit status: 0 = no caller reads any of the registers, 1 = a read (or unknown) was found.
"""
import os
import re
import struct
import sys

import elfscan

NAMES = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
         "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]
ARGS = set(range(4, 12))  # a0..a7 (EE EABI)
# Return-value registers: only v0. Survey (2026-11-18) of all 43,056 `jal` sites in
# SLES_520.48: v0 is read after 14,379 calls, v1 after none (the single hit was data decoded as
# code). ee-gcc returns even 64/128-bit values in v0, so v1 is dead after any call.
RETS = {2}
MAX_STEPS = 400

with open(elfscan.ELF, "rb") as _f:
    _f.seek(elfscan.SEG_OFF)
    SEG = _f.read(elfscan.SEG_SIZE)


def word(pc):
    off = pc - elfscan.SEG_VA
    return struct.unpack_from("<I", SEG, off)[0] if 0 <= off < len(SEG) - 3 else None


def decode(pc):
    """(reads, writes, kind, target, likely). kind: op, branch, jump, call, ret, jr, syscall,
    stop, unknown. reads/writes are sets of GPR numbers ($0 dropped)."""
    w = word(pc)
    if w is None:
        return set(), set(), "unknown", None, False
    op, rs, rt, rd = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
    funct = w & 63
    simm = (w & 0xFFFF) - 0x10000 if w & 0x8000 else w & 0xFFFF
    btarget = pc + 4 + (simm << 2)
    R = lambda *r: {x for x in r if x}
    if op == 0:  # SPECIAL
        if funct in (0, 2, 3) or 0x38 <= funct <= 0x3F:
            return R(rt), R(rd), "op", None, False
        if funct in (4, 6, 7, 0x14, 0x16, 0x17) or 0x20 <= funct <= 0x2F and funct not in (0x28, 0x29):
            return R(rs, rt), R(rd), "op", None, False
        if funct == 8:
            return R(rs), set(), "ret" if rs == 31 else "jr", None, False
        if funct == 9:
            return R(rs), R(rd), "call", None, False
        if funct in (0xA, 0xB):  # movz/movn keep rd when the condition fails
            return R(rs, rt, rd), R(rd), "op", None, False
        if funct == 0xC:
            return set(), set(), "syscall", None, False
        if funct == 0xD:
            return set(), set(), "stop", None, False
        if funct == 0xF:
            return set(), set(), "op", None, False
        if funct in (0x10, 0x12, 0x28):  # mfhi, mflo, mfsa
            return set(), R(rd), "op", None, False
        if funct in (0x11, 0x13, 0x29):  # mthi, mtlo, mtsa
            return R(rs), set(), "op", None, False
        if 0x18 <= funct <= 0x1B:  # mult/multu/div/divu (EE mult also writes rd)
            return R(rs, rt), R(rd), "op", None, False
        if 0x30 <= funct <= 0x36:  # traps
            return R(rs, rt), set(), "op", None, False
        return set(), set(), "unknown", None, False
    if op == 1:  # REGIMM
        if rt in (0, 1, 2, 3):
            return R(rs), set(), "branch", btarget, rt in (2, 3)
        if rt in (0x10, 0x11, 0x12, 0x13):
            return R(rs), {31}, "call", btarget, False
        if 8 <= rt <= 0xE or rt in (0x18, 0x19):
            return R(rs), set(), "op", None, False
        return set(), set(), "unknown", None, False
    if op in (2, 3):
        target = ((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        return set(), set(), "jump" if op == 2 else "call", target, False
    if op in (4, 0x14) and rs == rt:  # beq x,x (`b`): always taken, never falls through
        return R(rs), set(), "jump", btarget, False
    if op == 5 and rs == rt:  # bne x,x: never taken (its delay slot still runs)
        return R(rs), set(), "op", None, False
    if op in (4, 5, 0x14, 0x15):
        return R(rs, rt), set(), "branch", btarget, op >= 0x14
    if op in (6, 7, 0x16, 0x17):
        return R(rs), set(), "branch", btarget, op >= 0x16
    if 8 <= op <= 0xE or op in (0x18, 0x19):
        return R(rs), R(rt), "op", None, False
    if op == 0xF:  # lui
        return set(), R(rt), "op", None, False
    if op in (0x1A, 0x1B, 0x22, 0x26):  # ldl/ldr/lwl/lwr merge into rt
        # Compilers emit them in pairs (ldl+ldr, lwl+lwr) to load an unaligned value; the pair
        # replaces every byte, so the old rt is not really read. A lone half does merge.
        partner = {0x1A: 0x1B, 0x1B: 0x1A, 0x22: 0x26, 0x26: 0x22}[op]
        for k in (1, 2, 3, -1, -2, -3):
            other = word(pc + 4 * k)
            if other is not None and other >> 26 == partner and (other >> 16) & 31 == rt \
                    and (other >> 21) & 31 == rs:
                return R(rs), R(rt), "op", None, False
        return R(rs, rt), R(rt), "op", None, False
    if op == 0x1C:  # MMI: conservatively reads rs/rt/rd, writes rd
        return R(rs, rt, rd), R(rd), "op", None, False
    if op in (0x1E, 0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x37):  # lq and loads
        return R(rs), R(rt), "op", None, False
    if op in (0x1F, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x3F):  # sq and stores
        return R(rs, rt), set(), "op", None, False
    if op in (0x2F, 0x31, 0x33, 0x36, 0x39, 0x3E):  # cache, lwc1, pref, lqc2, swc1, sqc2
        return R(rs), set(), "op", None, False
    if op in (0x10, 0x11, 0x12):  # COP0 / COP1 / COP2
        if rs in (0, 1, 2):        # mfcN / qmfc2 / cfcN
            return set(), R(rt), "op", None, False
        if rs in (4, 5, 6):        # mtcN / qmtc2 / ctcN
            return R(rt), set(), "op", None, False
        if rs == 8:                # bcNf/t (+likely)
            return set(), set(), "branch", btarget, bool(rt & 2)
        if op == 0x10 and w & 0x02000000:  # COP0 eret etc.
            return set(), set(), "stop", None, False
        return set(), set(), "op", None, False  # FPU / VU0 arithmetic: no GPRs
    return set(), set(), "unknown", None, False


FUNCS = elfscan.load_functions()
STARTS = [a for a, _ in FUNCS]
MAX_DEPTH = 8
_memo = {}


def containing_function(pc):
    import bisect
    k = bisect.bisect_right(STARTS, pc) - 1
    return STARTS[k] if k >= 0 and pc < FUNCS[k][0] + FUNCS[k][1] else None


def walk(pc, reg, depth, in_callee):
    """('read' | 'dead' | 'unknown', pc) for `reg` from pc on.

    in_callee=False: pc follows a call in some caller. At `jr $ra` a v0/v1 value flows on to
    that function's own callers, which are checked in turn (up to MAX_DEPTH levels).
    in_callee=True: pc is the entry of a function that receives `reg` as a possible argument;
    returning without reading it means the value is dead (callers can't rely on scratch
    registers after a call)."""
    key = (pc, reg, in_callee)
    if key in _memo:
        return _memo[key]
    if depth > MAX_DEPTH:
        return "unknown", pc
    _memo[key] = ("dead", None)  # recursion guard: a cycle adds no new reads
    result = _walk(pc, reg, depth, in_callee)
    _memo[key] = result
    return result


def _walk(pc, reg, depth, in_callee):
    work, seen = [(pc, 0)], set()
    while work:
        pc, steps = work.pop()
        while True:
            if pc in seen:
                break
            seen.add(pc)
            steps += 1
            if steps > MAX_STEPS:
                return "unknown", pc
            reads, writes, kind, target, likely = decode(pc)
            if kind == "unknown":
                return "unknown", pc
            if reg in reads:
                return "read", pc
            if kind == "op":
                if reg in writes:
                    break
                pc += 4
                continue
            if kind == "syscall":
                break  # ABI rule: an argument the caller didn't write since our call is not passed
            if kind == "stop":
                break
            # Control transfer: the delay slot runs first (for likely branches only when taken).
            d_reads, d_writes, d_kind, _, _ = decode(pc + 4)
            if d_kind == "unknown":
                return "unknown", pc + 4
            if kind == "branch":
                if not likely:
                    if reg in d_reads:
                        return "read", pc + 4
                    if reg in d_writes:
                        break
                work.append((pc + 8, steps))  # not taken (likely: delay slot skipped)
                if likely:
                    if reg in d_reads:
                        return "read", pc + 4
                    if reg in d_writes:
                        continue  # taken path killed it; fall-through path is queued
                pc = target
                continue
            if reg in d_reads:
                return "read", pc + 4
            if reg in d_writes and kind != "jr":
                break
            if kind == "jump" and (word(pc) >> 26) in (4, 0x14):  # `b`: continue at the target
                if reg in d_writes:
                    break
                pc = target
                continue
            if kind in ("call", "jump"):
                if reg in ARGS and target is not None:  # direct call: look inside the callee
                    v, at = walk(target, reg, depth + 1, True)
                    if v != "dead":
                        return v, at
                break  # clobbered by the callee / not visible after the call
            if kind == "ret":
                if reg in RETS and not in_callee:
                    func = containing_function(pc)
                    if func is None:
                        return "unknown", pc
                    sites, jumps = call_sites(func)
                    if jumps or address_taken(func):
                        return "unknown", pc  # callers we can't enumerate
                    for s in sites:
                        v, at = walk(s + 8, reg, depth + 1, False)
                        if v != "dead":
                            return v, at
                break
            # jr through a register (a switch): not followed. Dead by the ABI rule below.
            break
    return "dead", None



def usage_after(pc, reg):
    return walk(pc, reg, 0, False)


import functools


@functools.lru_cache(maxsize=None)
def call_sites(target):
    sites, jumps = [], []  # (tuples: cached)
    for i in range(0, len(SEG) - 3, 4):
        w = struct.unpack_from("<I", SEG, i)[0]
        if w >> 26 in (2, 3):
            pc = elfscan.SEG_VA + i
            if (((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)) == target:
                (sites if w >> 26 == 3 else jumps).append(pc)
    return tuple(sites), tuple(jumps)


@functools.lru_cache(maxsize=None)
def address_taken(target):
    """Places that use the function's address as a value (function pointers, vtables)."""
    refs = []
    packed = struct.pack("<I", target)
    i = SEG.find(packed)
    while i >= 0:
        if i % 4 == 0:
            refs.append(f"data 0x{elfscan.SEG_VA + i:06x}")
        i = SEG.find(packed, i + 1)
    hi, lo = (target + 0x8000) >> 16, target & 0xFFFF
    lui = {}
    for i in range(0, len(SEG) - 3, 4):
        w = struct.unpack_from("<I", SEG, i)[0]
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        if op == 0xF:
            lui[rt] = (w & 0xFFFF, i)
        elif op == 9 and rs in lui and lui[rs][0] == hi and (w & 0xFFFF) == lo and i - lui[rs][1] < 96:
            refs.append(f"code 0x{elfscan.SEG_VA + i:06x}")
    return tuple(refs)


def scratch_from_result(name):
    path = os.path.join(elfscan.REPO, "out/theseus/difftest", name, "result.txt")
    if not os.path.exists(path):
        sys.exit(f"caller_usage: {path} not found; run difftest.py first or name the registers")
    line = next((l for l in open(path) if l.startswith("differing_fields:")), "")
    return [f.split("=")[0] for f in line.split()[1:] if "(ABI!)" not in f and f.split("=")[0] in NAMES]


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    name = sys.argv[1]
    m = re.search(r"([0-9a-fA-F]{6,8})$", name)
    target = int(m.group(1), 16)
    regs = sys.argv[2:] or scratch_from_result(name)
    copies = elfscan.rewrite_copies(name)  # identical copies share the rewrite: check their callers too
    sites, jumps, taken = [], [], []
    for t in [target] + copies:
        s, j = call_sites(t)
        sites += s
        jumps += j
        taken += address_taken(t)
    print(f"0x{target:06x}" + (f" + {len(copies)} copies" if copies else "")
          + f": {len(sites)} call sites (jal), {len(jumps)} tail jumps (j); checking {', '.join(regs) or 'nothing'}")
    bad = 0
    for reg in regs:
        n = NAMES.index(reg)
        results = [(s, *usage_after(s + 8, n)) for s in sites]
        reads = [(s, at) for s, v, at in results if v == "read"]
        unknown = [(s, at) for s, v, at in results if v == "unknown"]
        if n in RETS:
            # After a tail jump, our return goes to the jumper's caller: not followed yet.
            unknown += [(j, j) for j in jumps]
        status = "OK" if not (reads or unknown) else "READ" if reads else "UNKNOWN"
        bad += status != "OK"
        print(f"  {reg:3s} {status:8s} dead at {len(results) - len(reads) - len(unknown)}/{len(results)} sites"
              + "".join(f"\n      read after call at 0x{s:06x} (at 0x{at:06x})" for s, at in reads[:8])
              + "".join(f"\n      unknown after call at 0x{s:06x} (stopped at 0x{at:06x})" for s, at in unknown[:8]))
    if taken:
        print(f"  WARNING address taken ({len(taken)}): {', '.join(taken[:6])}"
              " - indirect callers are not checked")
    print("RESULT", "PASS" if not bad else "FAIL")
    sys.exit(0 if not bad else 1)


if __name__ == "__main__":
    main()

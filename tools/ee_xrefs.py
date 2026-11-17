#!/usr/bin/env python3
"""Find references to EE addresses in a PS2 ELF (MIPS R5900, little endian).

Code refs: tracks `lui rX, hi` per register within a short window and reports
`addiu/ori/lw/sw/...  rY, lo(rX)` pairs whose combined address hits a target.
Data refs: scans every aligned 32-bit word of the LOAD segment for the target.

Usage: ee_xrefs.py <elf> <addr> [<addr> ...]   (hex, virtual addresses)
       add --range to match any address in [addr, addr+0x40) instead.
"""
import re
import struct
import subprocess
import sys

WINDOW = 24  # instructions a lui value stays "live" for


def load_segment(elf):
    out = subprocess.run(["readelf", "-lW", elf], capture_output=True, text=True, check=True).stdout
    m = re.search(r"LOAD\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+0x[0-9a-f]+\s+0x([0-9a-f]+)", out)
    off, vaddr, size = (int(x, 16) for x in m.groups())
    with open(elf, "rb") as f:
        f.seek(off)
        return vaddr, f.read(size)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    ranged = "--range" in sys.argv
    elf, targets = args[0], [int(a, 16) for a in args[1:]]

    def hit(addr):
        for t in targets:
            if addr == t or (ranged and t <= addr < t + 0x40):
                return t
        return None

    vaddr, blob = load_segment(elf)
    for i in range(0, len(blob) - 3, 4):
        (w,) = struct.unpack_from("<I", blob, i)
        t = hit(w)
        if t is not None:
            print(f"data  0x{vaddr + i:08x}: .word 0x{w:08x}  -> 0x{t:x}")

    dis = subprocess.run(
        ["llvm-objdump", "-d", "--triple=mipsel", "--mcpu=mips3", "--no-show-raw-insn", elf],
        capture_output=True, text=True, check=True,
    ).stdout
    lui = {}  # reg -> (value, index)
    ins_re = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$")
    idx = 0
    for line in dis.splitlines():
        m = ins_re.match(line)
        if not m:
            continue
        idx += 1
        pc, op, operands = int(m.group(1), 16), m.group(2), m.group(3)
        ops = [o.strip() for o in operands.split(",")]
        if op == "lui" and len(ops) == 2:
            lui[ops[0]] = (int(ops[1], 0) << 16, idx)
            continue
        base = imm = None
        if op in ("addiu", "ori", "daddiu") and len(ops) == 3:
            base, imm = ops[1], int(ops[2], 0)
            if op == "ori":
                imm &= 0xFFFF
        elif len(ops) == 2 and "(" in ops[1]:
            mm = re.match(r"(-?(?:0x)?[0-9a-f]+)\((\$\w+)\)", ops[1])
            if mm:
                imm, base = int(mm.group(1), 0), mm.group(2)
        if base is None or base not in lui:
            continue
        hi, at = lui[base]
        if idx - at > WINDOW:
            continue
        addr = (hi + imm) & 0xFFFFFFFF
        t = hit(addr)
        if t is not None:
            print(f"code  0x{pc:08x}: {op} {operands}  -> 0x{addr:x}")


if __name__ == "__main__":
    main()

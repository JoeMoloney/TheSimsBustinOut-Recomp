"""Shared helpers for the Theseus tools: the game's function list and assert-string evidence.

The game ELF has no symbols or RTTI. What it does keep are assert messages that name the
source file (`c:/eor/src2/games/sims/MSrc/bstring.cpp`, `e_texture.h`, ...). A function that
builds the address of one of those strings with `lui` + `addiu/ori` references that file.
"""
import bisect
import csv
import os
import re
import struct

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
WORK = os.path.join(REPO, "work/sims-bustin-out")
ELF = os.path.join(WORK, "disc-files/SLES_520.48")
MAPS = [os.path.join(WORK, "SLES_520.48-recovered-map.csv"),
        os.path.join(REPO, "config/supplemental-map.csv")]

# Single LOAD segment of SLES_520.48 (readelf -lW): file offset, virtual address, file size.
SEG_OFF, SEG_VA, SEG_SIZE = 0x1000, 0x100000, 0x2F88A8


def load_functions():
    """[(start, size), ...] sorted by address; the recovered map wins over the supplemental map."""
    funcs = {}
    for path in MAPS:
        with open(path) as f:
            for row in csv.DictReader(f):
                funcs.setdefault(int(row["start"], 16), int(row["size"]))
    return [(a, funcs[a]) for a in sorted(funcs)]


def source_refs(funcs, pattern=r"([A-Za-z0-9_]+\.(?:cpp|h))\b"):
    """{function index: set of source file names it references via lui+addiu/ori}."""
    with open(ELF, "rb") as f:
        f.seek(SEG_OFF)
        seg = f.read(SEG_SIZE)
    strings = {}
    for m in re.finditer(rb"[\x20-\x7e]{4,}", seg):
        name = re.search(pattern, m.group().decode())
        if name:
            strings[SEG_VA + m.start()] = name.group(1)
    starts = [a for a, _ in funcs]
    refs = {}
    lui = {}  # register -> (upper value, pc of the lui)
    for i in range(0, len(seg) - 3, 4):
        w = struct.unpack_from("<I", seg, i)[0]
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        pc = SEG_VA + i
        if op == 0x0F:  # lui
            lui[rt] = (imm << 16, pc)
        elif op in (0x09, 0x0D) and rs in lui and pc - lui[rs][1] < 96:  # addiu / ori
            lo = imm - 0x10000 if (op == 0x09 and imm & 0x8000) else imm
            name = strings.get((lui[rs][0] + lo) & 0xFFFFFFFF)
            if name:
                k = bisect.bisect_right(starts, pc) - 1
                if k >= 0 and pc < funcs[k][0] + funcs[k][1]:
                    refs.setdefault(k, set()).add(name)
    return refs


def fill_source_files(n, refs, max_gap=60):
    """Best-guess source file per function index (prefer .cpp over headers).

    The linker keeps one .cpp file's functions together, so functions sitting between two
    functions of the same file (at most max_gap apart) are given that file too.
    Returns (direct, filled): lists of file name or None.
    """
    direct = [None] * n
    for k, names in refs.items():
        cpp = sorted(x for x in names if x.endswith(".cpp"))
        direct[k] = (cpp or sorted(names))[0]
    filled = list(direct)
    labelled = [k for k in range(n) if direct[k]]
    for a, b in zip(labelled, labelled[1:]):
        if direct[a] == direct[b] and b - a <= max_gap:
            for k in range(a + 1, b):
                filled[k] = direct[a]
    return direct, filled


# ---------------------------------------------------------------- identical copies
# A function the compiler emitted several times (inline copies, template instances) has
# byte-identical bodies. One rewrite then serves every copy: its source names them in a line
#     // theseus: copies 0x2cc050, 0x2cc0a0
# ship.py registers it at each one (after re-checking the bytes), caller_usage.py checks the
# callers of each, and progress.py colours each. Branches are PC-relative and leaves contain no
# absolute jumps out, so identical bytes mean identical behaviour.
REWRITES = os.path.join(REPO, "tools/theseus/rewrites")
RE_COPIES = re.compile(r"^//\s*theseus: copies\s+(.+)$", re.M)


def segment():
    with open(ELF, "rb") as f:
        f.seek(SEG_OFF)
        return f.read(SEG_SIZE)


def body(seg, addr, n_insns):
    return seg[addr - SEG_VA: addr - SEG_VA + 4 * n_insns]


def rewrite_copies(name):
    """Addresses listed in the rewrite's `// theseus: copies` line (empty if none)."""
    path = os.path.join(REWRITES, name + ".cpp")
    if not os.path.exists(path):
        return []
    m = RE_COPIES.search(open(path).read())
    return [int(a, 16) for a in re.findall(r"0x[0-9a-fA-F]+", m.group(1))] if m else []

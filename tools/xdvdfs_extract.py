#!/usr/bin/env python3
"""List / extract files from an Xbox disc image (XDVDFS; works on .xiso and full Redump .iso).

Usage:
    python3 tools/xdvdfs_extract.py <image> --list
    python3 tools/xdvdfs_extract.py <image> --out <new dir> [--ext .xbe .map ...]   (no --ext = everything)

Format: 2048-byte sectors. The volume descriptor ("MICROSOFT*XBOX*MEDIA") sits at sector 32 of the game
partition (offset 0 for an .xiso, 0x18300000 for a full Redump image). Each directory is a binary tree of
entries: u16 left, u16 right (child offsets in 4-byte units, 0 = none), u32 sector, u32 size, u8 attrs
(0x10 = directory), u8 name length, name; 0xFFFF marks padding to the end of a sector.
"""
import argparse
import os
import struct
import sys

SECTOR = 2048
MAGIC = b"MICROSOFT*XBOX*MEDIA"


def find_base(f):
    for base in (0, 0x18300000, 0x0FD90000, 0x02080000):  # xiso, Redump, older layouts
        f.seek(base + 32 * SECTOR)
        if f.read(len(MAGIC)) == MAGIC:
            return base
    sys.exit("no XDVDFS volume descriptor found")


def walk(f, base, sector, size, path=""):
    f.seek(base + sector * SECTOR)
    data = f.read(size)
    stack = [0] if size else []
    seen = set()
    while stack:
        off = stack.pop()
        if off in seen or off + 14 > len(data):
            continue
        seen.add(off)
        left, right, sec, sz, attr, nlen = struct.unpack_from("<HHIIBB", data, off)
        if left == 0xFFFF:
            continue
        name = data[off + 14:off + 14 + nlen].decode("latin-1")
        if left:
            stack.append(left * 4)
        if right:
            stack.append(right * 4)
        full = f"{path}/{name}"
        if attr & 0x10:
            yield from walk(f, base, sec, sz, full)
        else:
            yield full, sec, sz


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("image")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--out")
    ap.add_argument("--ext", nargs="*", help="only extract files with these extensions (case-insensitive)")
    a = ap.parse_args()
    with open(a.image, "rb") as f:
        base = find_base(f)
        f.seek(base + 32 * SECTOR + 0x14)
        root_sec, root_size = struct.unpack("<II", f.read(8))
        files = sorted(walk(f, base, root_sec, root_size))
        if a.list or not a.out:
            for p, sec, sz in files:
                print(f"{sz:12d}  {p}")
            print(f"{len(files)} files, {sum(s for _, _, s in files)} bytes", file=sys.stderr)
            return
        if os.path.exists(a.out) and os.listdir(a.out):
            sys.exit(f"{a.out} exists and is not empty; pass a new directory")
        exts = tuple(e.lower() for e in a.ext) if a.ext else None
        n = 0
        for p, sec, sz in files:
            if exts and not p.lower().endswith(exts):
                continue
            dst = os.path.join(a.out, p.lstrip("/"))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            f.seek(base + sec * SECTOR)
            with open(dst, "wb") as o:
                left = sz
                while left:
                    chunk = f.read(min(left, 1 << 20))
                    o.write(chunk)
                    left -= len(chunk)
            n += 1
        print(f"extracted {n} files to {a.out}", file=sys.stderr)


if __name__ == "__main__":
    main()

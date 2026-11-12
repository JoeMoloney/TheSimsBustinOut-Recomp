#!/usr/bin/env python3
"""Recover short MIPS callbacks referenced by allocated ELF data.

Outputs game-derived CSV only; keep it in an ignored local workspace. These
entries supplement, rather than replace, a proper disassembler function export.
"""

import argparse
import csv
import struct
from pathlib import Path


def control_flow(instruction):
    op = instruction >> 26
    return (
        op in (1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23)
        or (op == 0 and instruction & 63 in (8, 9, 12, 13))
        or (op in (16, 17, 18) and (instruction >> 21) & 31 == 8)
    )


def recover(elf_path, map_path, max_prefix=0):
    data = elf_path.read_bytes()
    if data[:6] != b"\x7fELF\x01\x01":
        raise ValueError("expected a little-endian ELF32 file")
    header = struct.unpack_from("<16sHHIIIIIHHHHHH", data)
    if header[2] != 8:
        raise ValueError("expected a MIPS executable")
    sections = [
        struct.unpack_from("<10I", data, header[6] + i * header[11])
        for i in range(header[12])
    ]
    with map_path.open(newline="") as stream:
        known = {int(row["start"], 0) for row in csv.DictReader(stream)}
    executable = [s for s in sections if s[1] == 1 and s[2] & 4]
    candidates = {}
    for section in sections:
        # Only file-backed, allocated, non-executable data contains candidates.
        if section[1] != 1 or not section[2] & 2 or section[2] & 4:
            continue
        for offset in range(section[4], section[4] + section[5] - 3, 4):
            address = struct.unpack_from("<I", data, offset)[0]
            if address in known or address % 4:
                continue
            for code in executable:
                if not code[3] <= address <= code[3] + code[5] - 8:
                    continue
                pos = code[4] + address - code[3]
                for prefix in range(max_prefix + 1):
                    current = pos + prefix * 4
                    if current + 8 > code[4] + code[5]:
                        break
                    instruction, delay = struct.unpack_from("<II", data, current)
                    # A return or direct tail jump terminates a straight-line
                    # callable region. Always include its delay-slot instruction.
                    if instruction == 0x03E00008 or instruction >> 26 == 2:
                        if not control_flow(delay):
                            candidates[address] = address + prefix * 4 + 8
                        break
                    if control_flow(instruction):
                        break
                break
    return sorted(candidates.items())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("existing_map", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--max-prefix", type=int, choices=range(0, 33), default=0,
                        help="maximum straight-line instructions before the final return/jump")
    args = parser.parse_args()
    entries = recover(args.elf, args.existing_map, args.max_prefix)
    # Refuse accidental overwrites; explicitly move an old map before rerunning.
    with args.output.open("x", newline="") as stream:
        writer = csv.writer(stream)
        writer.writerow(("name", "start", "end", "size"))
        for address, end in entries:
            writer.writerow((f"leaf_{address:08x}", hex(address), hex(end), end - address))
    print(f"Recovered {len(entries)} data-referenced callbacks into {args.output}")


if __name__ == "__main__":
    main()

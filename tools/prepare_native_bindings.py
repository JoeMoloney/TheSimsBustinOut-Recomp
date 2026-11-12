#!/usr/bin/env python3
"""Generate native SDK bindings from a locally verified address/symbol CSV.

The CSV header is address,symbol. Keep game-specific maps/output outside Git.
Only bind SDK functions positively identified from disassembly or symbols.
"""

import argparse
import csv
import re
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("bindings", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    with args.bindings.open(newline="") as stream:
        entries = list(csv.DictReader(stream))
    lines = ['#include "ps2_runtime.h"', '#include "ps2_stubs.h"', '#include "ps2_syscalls.h"', '#include "iop_buffer_bridge.h"', '#include <stdexcept>']
    addresses = set()
    for row in entries:
        address = int(row["address"], 0)
        symbol = row["symbol"]
        if not re.fullmatch(r"ps2_(?:stubs|syscalls|native)::[A-Za-z][A-Za-z0-9_]*", symbol):
            raise ValueError(f"invalid SDK symbol: {symbol}")
        if address & 3 or not 0 <= address <= 0xFFFFFFFF or address in addresses:
            raise ValueError(f"invalid or duplicate guest address: {address:#x}")
        addresses.add(address)
        lines.extend([
            f"static void native_{address:x}(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime) {{",
            "    ctx->pc = getRegU32(ctx, 31);",
            f"    {symbol}(rdram, ctx, runtime);",
            "}",
        ])
    lines.append("void registerNativeBindings(PS2Runtime &runtime) {")
    for row in entries:
        address = int(row["address"], 0)
        lines.extend([
            f"    if (!runtime.registerFunction(0x{address:x}u, native_{address:x}))",
            f'        throw std::runtime_error("SDK binding outside main table: 0x{address:x}");',
        ])
    lines.append("}")
    content = "\n".join(lines) + "\n"
    if not args.output.exists() or args.output.read_text() != content:
        args.output.write_text(content)
    print(f"Prepared {len(entries)} native SDK bindings in {args.output}")


if __name__ == "__main__":
    main()

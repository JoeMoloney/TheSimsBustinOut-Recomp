#!/usr/bin/env python3
"""Combine PS2Recomp supplemental output without its duplicate entry/table.

Run after recompiling a supplemental map which includes the real ELF entry.
Link supplemental.cpp alongside the main generated game objects, then call
registerSupplementalFunctions(runtime) before execution. Keep generated output
outside Git. Resumable entries are taken from the generated registration table.
"""

import argparse
import re
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_directory", type=Path)
    args = parser.parse_args()
    directory = args.output_directory
    header = (directory / "ps2_recompiled_functions.h").read_text()
    functions = re.findall(r"^void (\w+_0x([0-9a-f]+))\(", header, re.MULTILINE)
    functions = [(name, address) for name, address in functions if not name.startswith("entry_")]
    names = {name for name, _ in functions}
    registration = (directory / "register_functions.cpp").read_text()
    entries = re.findall(
        r"g_ps2RecompiledFunctionTable\[\d+\] = (\w+); // 0x([0-9a-f]+)",
        registration,
    )
    lines = ["#include <ps2_runtime.h>", "#include <stdexcept>"]
    lines.extend(f'#include "{name}.cpp"' for name, _ in functions)
    lines.append("void registerSupplementalFunctions(PS2Runtime &runtime) {")
    for name, address in entries:
        if name in names:
            lines.append(f"    if (!runtime.registerFunction(0x{address}u, {name}))")
            lines.append(f'        throw std::runtime_error("Supplemental entry outside main table: 0x{address}");')
    lines.append("}")
    output = directory / "supplemental.cpp"
    content = "\n".join(lines) + "\n"
    if not output.exists() or output.read_text() != content:
        output.write_text(content)
    print(f"Prepared {len(functions)} supplemental functions in {output}")


if __name__ == "__main__":
    main()

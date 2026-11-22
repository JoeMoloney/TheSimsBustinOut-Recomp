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
    parser.add_argument("--main-output", type=Path,
                        help="main generated output; its function table is widened to fit supplemental entries")
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
    if args.main_output:
        # Cover each supplemental function's whole body (its "// Address: A - B"
        # comment), not only its entry, as the earlier hand edit did.
        ends = [int(address, 16) + 4 for name, address in entries if name in names]
        for name, _ in functions:
            source = (directory / f"{name}.cpp").read_text(errors="replace")
            span = re.search(r"^// Address: 0x[0-9a-f]+ - 0x([0-9a-f]+)", source, re.MULTILINE)
            if span:
                ends.append(int(span.group(1), 16))
        widen_main_table(args.main_output / "register_functions.cpp", ends)


def widen_main_table(path, ends):
    """Grow the main dense function table to reach every end address given.

    PS2Recomp sizes the table to end 4 bytes after the main map's last entry,
    so a supplemental function past that (0x39f098 in this game) cannot be
    registered. Regenerating the main output resets the size, hence this step.
    """
    if not ends:
        return
    text = path.read_text()
    base = int(re.search(r"g_ps2RecompiledFunctionTableBase = 0x([0-9a-f]+)u;", text).group(1), 16)
    end = int(re.search(r"g_ps2RecompiledFunctionTableEnd = 0x([0-9a-f]+)u;", text).group(1), 16)
    slots = int(re.search(r"g_ps2RecompiledFunctionTableSlotCount = (\d+)u;", text).group(1))
    needed = (max(ends) + 3) & ~3
    if needed <= end:
        print(f"Main table already covers supplemental entries (end 0x{end:x})")
        return
    new_slots = (needed - base) >> 2
    text = text.replace(f"g_ps2RecompiledFunctionTableEnd = 0x{end:x}u;", f"g_ps2RecompiledFunctionTableEnd = 0x{needed:x}u;")
    text = text.replace(f"g_ps2RecompiledFunctionTableSlotCount = {slots}u;", f"g_ps2RecompiledFunctionTableSlotCount = {new_slots}u;")
    text = text.replace(f"g_ps2RecompiledFunctionTable[{slots}u]", f"g_ps2RecompiledFunctionTable[{new_slots}u]")
    path.write_text(text)
    print(f"Widened main table end 0x{end:x} -> 0x{needed:x} ({slots} -> {new_slots} slots) in {path}")


if __name__ == "__main__":
    main()

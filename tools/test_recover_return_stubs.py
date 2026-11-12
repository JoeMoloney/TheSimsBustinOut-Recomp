"""Synthetic ELF tests; no game data is required."""

import csv
import struct
import tempfile
import unittest
from pathlib import Path

from recover_return_stubs import recover


class ReturnStubTests(unittest.TestCase):
    def test_only_data_referenced_safe_returns_are_recovered(self):
        data = bytearray(0x300)
        ident = b"\x7fELF\x01\x01\x01" + bytes(9)
        struct.pack_into("<16sHHIIIIIHHHHHH", data, 0, ident, 2, 8, 1,
                         0x1000, 0, 0x200, 0, 52, 0, 0, 40, 3, 0)
        # Section zero is empty, followed by executable code and allocated data.
        struct.pack_into("<10I", data, 0x228, 0, 1, 6, 0x1000, 0x100, 32, 0, 0, 4, 0)
        struct.pack_into("<10I", data, 0x250, 0, 1, 2, 0x2000, 0x120, 16, 0, 0, 4, 0)
        struct.pack_into("<8I", data, 0x100,
                         0x03E00008, 0x24020001,  # return one
                         0x03E00008, 0x10000000,  # unsafe branch delay slot
                         0x03E00008, 0,           # already known
                         0x03E00008, 0)           # not referenced
        struct.pack_into("<4I", data, 0x120, 0x1000, 0x1008, 0x1010, 0x1004)
        workspace = Path(__file__).resolve().parents[1] / "work"
        workspace.mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=workspace) as temporary:
            root = Path(temporary)
            elf = root / "synthetic.elf"
            elf.write_bytes(data)
            function_map = root / "map.csv"
            with function_map.open("w", newline="") as stream:
                writer = csv.writer(stream)
                writer.writerow(("name", "start", "end", "size"))
                writer.writerow(("known", "0x1010", "0x1018", 8))
            self.assertEqual(recover(elf, function_map), [(0x1000, 0x1008)])

            # A three-instruction constant-return leaf includes its delay slot.
            struct.pack_into("<3I", data, 0x100, 0x3C020001, 0x03E00008, 0x34428000)
            struct.pack_into("<4I", data, 0x120, 0x1000, 0x1010, 0, 0)
            elf.write_bytes(data)
            self.assertEqual(recover(elf, function_map), [])
            self.assertEqual(recover(elf, function_map, max_prefix=1), [(0x1000, 0x100C)])

            # A direct tail-jump thunk also has an exact two-word boundary.
            struct.pack_into("<2I", data, 0x100, 0x08000800, 0)
            elf.write_bytes(data)
            self.assertEqual(recover(elf, function_map), [(0x1000, 0x1008)])

    def test_rejects_non_elf(self):
        workspace = Path(__file__).resolve().parents[1] / "work"
        workspace.mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=workspace) as temporary:
            path = Path(temporary) / "invalid"
            path.write_bytes(b"not an ELF")
            with self.assertRaisesRegex(ValueError, "ELF32"):
                recover(path, path)


if __name__ == "__main__":
    unittest.main()

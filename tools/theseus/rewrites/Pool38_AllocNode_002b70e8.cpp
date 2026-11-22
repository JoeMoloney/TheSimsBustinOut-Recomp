// Takes a 0x38-byte node from the pool a0 (u16 free index +0x1092, chained through each free node'\''s u16 +0; node array +0x1094; count +0x109C incremented) and initialises it: u16 +0x20 = index of a3 in the array (a3 - array) / 0x38, or 0 for a3 == 0; u16 +0x22..+0x30 = 0; byte +0x35 = a1, +0x33 = a2, +0x32 = 0; +0..+0xB = 12 bytes copied from a4 (t0); +0xC..+0x1F = 0. Returns the node. With an empty pool (index 0) the node is address 0 and the original writes there all the same. 2 call sites.
#include "theseus.h"

void Pool38_AllocNode_002b70e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t pool = g.arg(0);
    const uint16_t index = g.read16(pool + 0x1092);  // gate: equivalent read32: truncated to uint16_t
    if (index != 0)
        g.write16(pool + 0x1092, g.read16(g.read32(pool + 0x1094) + index * 0x38u));  // gate: equivalent read32: stored as a halfword
    const uint32_t node = index != 0 ? g.read32(pool + 0x1094) + index * 0x38u : 0;
    g.write32(pool + 0x109C, g.read32(pool + 0x109C) + 1);
    uint16_t back = 0;
    if (g.arg64(3) != 0)
    {
        // (a3 - array) / 0x38 as the original computes it: * 0xB6DB6DB7 (inverse of 7), >> 3
        const uint32_t product = (g.arg(3) - g.read32(pool + 0x1094)) * 0xB6DB6DB7u;
        back = static_cast<uint16_t>(static_cast<int32_t>(product) >> 3);
    }
    g.write16(node + 0x20, back);  // gate: equivalent write32: the next halfword (+0x22) is zeroed by the loop below
    for (int32_t off = 0x30; off >= 0x22; off -= 2)
        g.write16(node + static_cast<uint32_t>(off), 0);  // gate: equivalent write32: the halfword after is zeroed next, or (+0x32/+0x33) stored below
    g.write8(node + 0x35, static_cast<uint8_t>(g.arg(1)));
    g.write8(node + 0x33, static_cast<uint8_t>(g.arg(2)));
    g.write8(node + 0x32, 0);
    const uint64_t head = g.read64(g.arg(4));
    const uint32_t tail = g.read32(g.arg(4) + 8);
    g.write64(node, head);
    g.write32(node + 8, tail);
    g.write32(node + 0x18, 0);
    g.write32(node + 0x1C, 0);
    g.write64(node + 0xC, 0);
    g.write32(node + 0x14, 0);
    g.ret(node);
}

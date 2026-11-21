// std::map<unsigned short, Obj*>::find on the map whose header is [a0+4] (root header+4; node: left +8, right +0xC, key u16 +0x10, value +0x14). If found, sets the value'\''s +0x80 and a0+0x20 to 1 and returns 1; else returns the header. 3 call sites (2 identical copies).
// theseus: copies 0x33ac50
#include "theseus.h"

void Map16_FindSetFlag80_0033abc8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t header = g.read32(g.arg(0) + 4);
    const uint16_t key = static_cast<uint16_t>(g.arg(1));
    uint32_t lower = header;
    for (uint32_t node = g.read32(header + 4); node != 0;)
    {
        if (g.read16(node + 0x10) < key)
            node = g.read32(node + 0xC);
        else
        {
            lower = node;
            node = g.read32(node + 8);
        }
    }
    const uint32_t end = g.read32(g.arg(0) + 4);
    if (lower == end || key < g.read16(lower + 0x10))
    {
        g.ret(g.read32(g.arg(0) + 4));
        return;
    }
    g.write32(g.read32(lower + 0x14) + 0x80, 1);
    g.write32(g.arg(0) + 0x20, 1);
    g.ret(1);
}

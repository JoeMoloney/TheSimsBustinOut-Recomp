// std::map<u16, T>::find on the map whose header is [a0+4] (SGI tree: parent +4, left +8, right +0xC,
// key u16 at +0x10, value at +0x14): the value for the key (a1 & 0xFFFF), or 0 if it is missing or
// the key is above 0xFFFA. (The original also spills the key and two pointers to its stack frame.)
// 22 call sites.
#include "theseus.h"

void MapU16_FindValue14_0033b1c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t key = g.arg(1) & 0xFFFF;
    if (key > 0xFFFA)
    {
        g.ret(0);
        return;
    }
    const uint32_t header = g.read32(g.arg(0) + 4);
    uint32_t best = header;  // lower bound
    for (uint32_t node = g.read32(header + 4); node != 0;)
    {
        if (g.read16(node + 0x10) < key)
            node = g.read32(node + 0xC);
        else
        {
            best = node;
            node = g.read32(node + 8);
        }
    }
    if (best != header && key < g.read16(best + 0x10))
        best = header;
    g.ret(best == g.read32(g.arg(0) + 4) ? 0 : g.read32(best + 0x14));
}

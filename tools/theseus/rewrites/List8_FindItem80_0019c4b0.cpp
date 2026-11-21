// Walks the list whose head is a0+0x19C (node +0 = item, +8 = next) and returns the first item
// whose field +0x80 equals a1 (all 64 bits of a1), or 0. 2 call sites.
#include "theseus.h"

void List8_FindItem80_0019c4b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    for (uint32_t node = g.read32(g.arg(0) + 0x19C); node != 0; node = g.read32(node + 8))
    {
        const uint32_t item = g.read32(node);
        if (static_cast<int32_t>(g.read32(item + 0x80)) == key)
        {
            g.ret(item);
            return;
        }
    }
    g.ret(0);
}

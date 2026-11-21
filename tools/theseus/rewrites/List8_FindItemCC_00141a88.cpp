// Walks a list (head at *a0; node +0 = item, +8 = next) and returns the first item whose field
// +0xCC equals a1 (compared on all 64 bits of a1), or 0. 2 call sites.
#include "theseus.h"

void List8_FindItemCC_00141a88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    for (uint32_t node = g.read32(g.arg(0)); node != 0; node = g.read32(node + 8))
    {
        const uint32_t item = g.read32(node);
        if (static_cast<int32_t>(g.read32(item + 0xCC)) == key)
        {
            g.ret(item);
            return;
        }
    }
    g.ret(0);
}

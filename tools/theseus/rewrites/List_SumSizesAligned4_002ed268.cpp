// Walks the list from a0 (+0 next, +4 item or 0). For each item, rounds the running total up to a multiple of 4 and adds the item'\''s size (item +0); counts the items into *a1. Returns the total. 3 call sites.
#include "theseus.h"

void List_SumSizesAligned4_002ed268(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t total = 0, count = 0;
    if (g.arg64(0) != 0)  // gate: equivalent 1: the loop's own node != 0 test covers a0 == 0
    {
        for (uint32_t node = g.arg(0); node != 0;)
        {
            const uint32_t item = g.read32(node + 4);
            node = g.read32(node);
            if (item != 0)
            {
                total = ((total + 3) & ~3u) + g.read32(item);
                ++count;
            }
        }
    }
    g.write32(g.arg(1), count);
    g.ret(total);
}

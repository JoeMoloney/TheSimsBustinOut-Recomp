// Takes an item from the free list of the pool [0x3F7D78] ($gp - 0x7D78; the pool'\''s +0 is the list head, links at +0) and increments the counter 0x3F7D7C. Returns the item, or 0 when there is no pool or it is empty. 2 call sites.
#include "theseus.h"

void Pool3F7D78_Alloc_00172c78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t pool = g.read32(0x3F7D78);
    if (pool == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t item = g.read32(pool);
    if (item == 0)
    {
        g.ret(0);
        return;
    }
    g.write32(pool, g.read32(item));
    g.write32(0x3F7D7C, g.read32(0x3F7D7C) + 1);
    g.ret(item);
}

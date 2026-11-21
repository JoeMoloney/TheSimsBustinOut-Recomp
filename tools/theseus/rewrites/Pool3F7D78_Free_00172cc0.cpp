// Returns item a0 to the pool [0x3F7D78] ($gp - 0x7D78): decrements the counter 0x3F7D7C and pushes the item on the pool'\''s free list (+0 head, links +0). Nothing without a pool or for a0 == 0 (v0 then untouched); else returns the old head. 1 call site. Pairs with Pool3F7D78_Alloc_00172c78.
#include "theseus.h"

void Pool3F7D78_Free_00172cc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t pool = g.read32(0x3F7D78);
    if (pool == 0 || g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    g.write32(0x3F7D7C, g.read32(0x3F7D7C) - 1);
    const uint32_t old = g.read32(pool);
    g.write32(g.arg(0), old);
    g.write32(pool, g.arg(0));
    g.ret(old);
}

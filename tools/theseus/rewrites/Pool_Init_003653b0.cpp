// Initialises the pool at a0 unless [a0] (initialised flag) is set: +8 = a2, +0xC = a0+0x1C (the slot
// array), +4 = a1 (count), +0x18 = 0, and every slot = 0x80000000 (at least one slot is written
// when a1 > 0, all 64 bits; the loop bound is re-read from +4). Always sets [a0] = 1 and returns 1.
// 9 call sites (2 identical copies).
// theseus: copies 0x3654a8
#include "theseus.h"

void Pool_Init_003653b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t pool = g.arg(0);
    if (g.read32(pool) == 0)
    {
        g.write32(pool + 8, g.arg(2));
        g.write32(pool + 0xC, pool + 0x1C);
        g.write32(pool + 4, g.arg(1));
        g.write32(pool + 0x18, 0);
        if (g.sarg64(1) > 0)
        {
            int32_t i = 0;
            do
            {
                g.write32(g.read32(pool + 0xC) + 4 * static_cast<uint32_t>(i), 0x80000000);
                ++i;
            } while (i < static_cast<int32_t>(g.read32(pool + 4)));
        }
    }
    g.write32(pool, 1);
    g.ret(1);
}

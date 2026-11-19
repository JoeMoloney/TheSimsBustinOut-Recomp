// Compares the 16-bit string whose pointer is at a0 with the 16-bit string at a1: -1 / 0 / 1
// (unsigned characters). The first characters are compared before any end check; afterwards
// the strings end together (0) or the shorter one is smaller. 9 call sites.
#include "theseus.h"

void WStr16_Compare_002bbd78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.read32(g.arg(0)), q = g.arg(1);
    uint32_t c1 = g.read16(p);
    for (;;)
    {
        const uint32_t c2 = g.read16(q);
        if (c1 < c2)
        {
            g.ret(0xFFFFFFFFu);
            return;
        }
        if (c2 < c1)
        {
            g.ret(1);
            return;
        }
        p += 2;
        q += 2;
        c1 = g.read16(p);
        if (c1 == 0 && g.read16(q) == 0)
        {
            g.ret(0);
            return;
        }
    }
}

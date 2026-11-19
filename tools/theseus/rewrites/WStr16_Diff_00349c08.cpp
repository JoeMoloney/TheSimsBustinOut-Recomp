// Compares the 16-bit strings at a0 and a1: the difference of the first differing characters
// (unsigned, as a signed 32-bit value), or 0 if they are equal (3 call sites).
#include "theseus.h"

void WStr16_Diff_00349c08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), q = g.arg(1);
    for (;;)
    {
        const uint32_t c1 = g.read16(p), c2 = g.read16(q);
        if (c1 != c2)
        {
            g.ret(c1 - c2);
            return;
        }
        if (c1 == 0)  // c1 == c2 here
        {
            g.ret(0);
            return;
        }
        p += 2;
        q += 2;
    }
}

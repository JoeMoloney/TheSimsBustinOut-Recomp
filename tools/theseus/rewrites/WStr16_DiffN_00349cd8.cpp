// Compares at most n = a2 & 0xFFFF 16-bit characters of the strings at a0 and a1 (at least one:
// the count is checked after a comparison): the difference of the first differing characters
// (unsigned, as a signed 32-bit value), else 0 (3 call sites).
#include "theseus.h"

void WStr16_DiffN_00349cd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t limit = static_cast<int32_t>(g.arg(2) & 0xFFFF);
    uint32_t p = g.arg(0), q = g.arg(1);
    for (int32_t compared = 1;; ++compared)
    {
        const uint32_t c1 = g.read16(p), c2 = g.read16(q);
        if (c1 != c2)
        {
            g.ret(c1 - c2);
            return;
        }
        if (c1 == 0 || !(compared < limit))  // c1 == c2 here
        {
            g.ret(0);
            return;
        }
        p += 2;
        q += 2;
    }
}

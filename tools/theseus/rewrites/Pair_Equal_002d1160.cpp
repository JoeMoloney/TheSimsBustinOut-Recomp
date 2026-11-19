// True if the 8-byte pairs at a0 and a1 are equal: word +4 first, then word +0 (6 call sites).
#include "theseus.h"

void Pair_Equal_002d1160(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    if (g.read32(a + 4) != g.read32(b + 4))
    {
        g.ret(0);
        return;
    }
    g.ret(g.read32(a) == g.read32(b) ? 1 : 0);
}

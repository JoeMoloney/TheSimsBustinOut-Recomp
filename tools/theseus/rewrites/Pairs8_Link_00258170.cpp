// Links entries a1 and a2 of the 8-byte pair table at a0: pair[a1].+4 = a2, pair[a2].+0 = a1,
// then sets the word flag a0+0x10 + a1 * 4 to 1. Returns 1. 4 call sites.
#include "theseus.h"

void Pairs8_Link_00258170(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t base = g.arg(0), i = g.arg(1), j = g.arg(2);
    g.write32(base + (i << 3) + 4, j);
    g.write32(base + (j << 3), i);
    g.write32(base + (i << 2) + 0x10, 1);
    g.ret(1);
}

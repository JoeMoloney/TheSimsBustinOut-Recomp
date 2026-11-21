// Increments the word at a0+4 when the word at a0 is nonzero. Returns 1. 2 call sites.
#include "theseus.h"

void Field4_IncrementIfSet_0035ac80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0)) != 0)
        g.write32(g.arg(0) + 4, g.read32(g.arg(0) + 4) + 1);
    g.ret(1);
}

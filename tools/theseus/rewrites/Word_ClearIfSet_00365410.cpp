// Zeroes the word at a0 if it is nonzero (no store otherwise); returns 1. 4 call sites.
#include "theseus.h"

void Word_ClearIfSet_00365410(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0)) != 0)
        g.write32(g.arg(0), 0);
    g.ret(1);
}

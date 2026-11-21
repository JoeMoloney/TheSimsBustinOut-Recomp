// Sets [a0+0x84] = a1 and the flag [a0+0x80] = 1; returns 1. 41 call sites.
#include "theseus.h"

void Field84_SetValid_0021c2e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x84, g.arg(1));
    g.write32(g.arg(0) + 0x80, 1);
    g.ret(1);
}

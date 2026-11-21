// Sets [a0+0x18] = a1 - 1. 15 call sites.
#include "theseus.h"

void Field18_SetMinus1_002ed7c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x18, g.arg(1) - 1);
    g.ret_void();
}

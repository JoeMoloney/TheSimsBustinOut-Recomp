// Stores a2 in the word array at a0+0x14, index a1. 3 call sites.
#include "theseus.h"

void Words14_SetAt_00341dc8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + (g.arg(1) << 2) + 0x14, g.arg(2));
    g.ret_void();
}

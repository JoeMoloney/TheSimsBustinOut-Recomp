// Stores a2 in the word array at a0+0x88, index a1. 2 call sites.
#include "theseus.h"

void Words88_SetAt_00361560(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + ((g.arg(1) + 0xA) << 2) + 0x60, g.arg(2));
    g.ret_void();
}

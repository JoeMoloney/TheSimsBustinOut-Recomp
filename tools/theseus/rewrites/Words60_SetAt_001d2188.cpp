// Stores a2 in the word array at a0+0x60, index a1. 1 call site.
#include "theseus.h"

void Words60_SetAt_001d2188(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + (g.arg(1) << 2) + 0x60, g.arg(2));
    g.ret_void();
}

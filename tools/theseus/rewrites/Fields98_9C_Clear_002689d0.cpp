// Zeroes the words at a0+0x9C and a0+0x98. 1 call site.
#include "theseus.h"

void Fields98_9C_Clear_002689d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x9C, 0);
    g.write32(g.arg(0) + 0x98, 0);
    g.ret_void();
}

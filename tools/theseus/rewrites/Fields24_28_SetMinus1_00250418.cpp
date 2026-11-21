// Sets the words at a0+0x24 and a0+0x28 to -1; returns -1. 2 call sites.
#include "theseus.h"

void Fields24_28_SetMinus1_00250418(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x24, 0xFFFFFFFFu);
    g.write32(g.arg(0) + 0x28, 0xFFFFFFFFu);
    g.ret(0xFFFFFFFFu);
}

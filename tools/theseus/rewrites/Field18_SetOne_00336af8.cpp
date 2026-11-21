// Sets the word at a0+0x18 to 1 and returns 1. 3 call sites.
#include "theseus.h"

void Field18_SetOne_00336af8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x18, 1);
    g.ret(1);
}

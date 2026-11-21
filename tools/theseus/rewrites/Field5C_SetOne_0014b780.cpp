// Sets the word at a0+0x5C to 1 and returns 1. 7 call sites.
#include "theseus.h"

void Field5C_SetOne_0014b780(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x5C, 1);
    g.ret(1);
}

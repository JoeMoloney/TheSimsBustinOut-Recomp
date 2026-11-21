// Sets the word at a0+4 to 1 and returns 1. 8 call sites.
#include "theseus.h"

void Field4_SetOne_00202018(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 1);
    g.ret(1);
}

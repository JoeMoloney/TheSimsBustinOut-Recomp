// The sum of the signed bytes at a0 and a0+1. 1 call site.
#include "theseus.h"

void SByte01_Sum_002cc220(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<int32_t>(g.sread8(g.arg(0))) + g.sread8(g.arg(0) + 1));
}

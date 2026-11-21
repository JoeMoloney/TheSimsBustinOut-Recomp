// The signed byte at a0 minus the signed byte at a0+1. 1 call site.
#include "theseus.h"

void SByte01_Diff_002cc230(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<int32_t>(g.sread8(g.arg(0))) - g.sread8(g.arg(0) + 1));
}

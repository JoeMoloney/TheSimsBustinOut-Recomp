// Stores a1 at a0+0x38 and returns 1. 1 call site.
#include "theseus.h"

void Field38_SetRetOne_00360968(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x38, g.arg(1));
    g.ret(1);
}

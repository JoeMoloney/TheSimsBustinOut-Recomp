// Stores a1 at a0+4 and the tag byte 7 at a0; returns 7. 1 call site.
#include "theseus.h"

void Tagged7_Set_002704a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, g.arg(1));
    g.write8(g.arg(0), 7);
    g.ret(7);
}

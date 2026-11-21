// Sets the globals 0x3F8F54 = a0, 0x3F8278 ($gp - 0x7878) = 1, 0x3F8F58 = a1, 0x3F8F50 = a0. Returns 1. 1 call site.
#include "theseus.h"

void Globals3F8F50_SetPair_002971c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F8F54, g.arg(0));
    g.write32(0x3F8278, 1);
    g.write32(0x3F8F58, g.arg(1));
    g.write32(0x3F8F50, g.arg(0));
    g.ret(1);
}

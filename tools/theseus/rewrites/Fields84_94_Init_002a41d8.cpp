// a0+0x94 = 0, +0x84 = 1, +0x8C/+0x90/+0x88 = 0. Returns 1. 1 call site.
#include "theseus.h"

void Fields84_94_Init_002a41d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x94, 0);
    g.write32(p + 0x84, 1);
    g.write32(p + 0x8C, 0);
    g.write32(p + 0x90, 0);
    g.write32(p + 0x88, 0);
    g.ret(1);
}

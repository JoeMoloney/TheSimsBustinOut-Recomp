// a0+0x148 = a3, +0x140 = a1, +0x138 = 1, +0x144 = a2; returns 1. 4 call sites.
#include "theseus.h"

void Fields138_148_Set_00255b60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    g.write32(obj + 0x148, g.arg(3));
    g.write32(obj + 0x140, g.arg(1));
    g.write32(obj + 0x138, 1);
    g.write32(obj + 0x144, g.arg(2));
    g.ret(1);
}

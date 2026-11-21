// a0+0x5D4 = a2, +0x5D0 = a1, +0x5DC = 0, +0x5E0 = a1. 2 call sites.
#include "theseus.h"

void Fields5D0_5E0_Set_00159180(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x5D4, g.arg(2));
    g.write32(p + 0x5D0, g.arg(1));
    g.write32(p + 0x5DC, 0);
    g.write32(p + 0x5E0, g.arg(1));
    g.ret_void();
}

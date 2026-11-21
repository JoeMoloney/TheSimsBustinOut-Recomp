// a0+0x70 = a1, +0x78 = a2, +0x74 = 0, +0x6C = a1. 1 call site.
#include "theseus.h"

void Fields6C_78_Set_002a53b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x70, g.arg(1));
    g.write32(p + 0x78, g.arg(2));
    g.write32(p + 0x74, 0);
    g.write32(p + 0x6C, g.arg(1));
    g.ret_void();
}

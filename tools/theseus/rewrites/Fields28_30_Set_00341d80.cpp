// a0+0x30 = a3, +0x28 = a1, +0x2C = a2. 2 call sites.
#include "theseus.h"

void Fields28_30_Set_00341d80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x30, g.arg(3));
    g.write32(g.arg(0) + 0x28, g.arg(1));
    g.write32(g.arg(0) + 0x2C, g.arg(2));
    g.ret_void();
}

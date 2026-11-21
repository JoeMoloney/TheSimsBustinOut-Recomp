// Stores a1, a2, a3, a4 (t0), a5 (t1) at a0+0, +4, +8, +0xC, +0x10 (in the original's order). Returns a0. 3 call sites.
#include "theseus.h"

void Words5_Set_003704c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 8, g.arg(3));
    g.write32(p + 4, g.arg(2));
    g.write32(p + 0xC, g.arg(4));
    g.write32(p + 0x10, g.arg(5));
    g.ret_copy_of_arg(0);
}

// Initialiser: +8 = 1, +0x10 = a1 * 2, +4 = 0, +0 = 0, +0xC = a1. Returns a0. 4 call sites.
#include "theseus.h"

void Object_InitSize_002ac1a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0), n = g.arg(1);
    g.write32(p + 8, 1);
    g.write32(p + 0x10, n << 1);
    g.write32(p + 4, 0);
    g.write32(p, 0);
    g.write32(p + 0xC, n);
    g.ret_copy_of_arg(0);
}

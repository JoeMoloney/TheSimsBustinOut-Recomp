// Initialiser: +0 = a1, +8 = a2, +4 = 0, +0x1C = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object20_InitPair_0031f7c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 8, g.arg(2));
    g.write32(p + 4, 0);
    g.write32(p + 0x1C, 0);
    g.ret_copy_of_arg(0);
}

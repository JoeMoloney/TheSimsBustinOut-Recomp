// Initialiser: +0 = a1, +8 = -1, +0xC = a2, +4 = 0, +0x10 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object14_Init_0028d0a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 8, 0xFFFFFFFFu);
    g.write32(p + 0xC, g.arg(2));
    g.write32(p + 4, 0);
    g.write32(p + 0x10, 0);
    g.ret_copy_of_arg(0);
}

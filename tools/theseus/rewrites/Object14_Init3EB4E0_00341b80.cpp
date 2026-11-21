// Initialiser: +0xC = -1, +0x10 = 0x3EB4E0, +0, +4, +8 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object14_Init3EB4E0_00341b80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, 0xFFFFFFFFu);
    g.write32(p + 0x10, 0x3EB4E0);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.ret_copy_of_arg(0);
}

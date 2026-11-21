// Initialiser: +4 = 0, +8 = 0x3A8AE8, +0 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object_Init3A8AE8_002bc408(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0x3A8AE8);
    g.write32(p, 0);
    g.ret_copy_of_arg(0);
}

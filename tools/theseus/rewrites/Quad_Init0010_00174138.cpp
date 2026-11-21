// Initialises four words: +0 = 0, +4 = 0, +8 = 1, +0xC = 0. Returns a0. 4 call sites.
#include "theseus.h"

void Quad_Init0010_00174138(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 4, 0);
    g.write32(p + 8, 1);
    g.write32(p, 0);
    g.write32(p + 0xC, 0);
    g.ret_copy_of_arg(0);
}

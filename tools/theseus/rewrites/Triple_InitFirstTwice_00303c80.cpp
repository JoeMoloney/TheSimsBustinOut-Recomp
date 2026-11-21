// Initialiser: +4 = a1, +8 = a2, +0 = a1. Returns a0. 2 call sites.
#include "theseus.h"

void Triple_InitFirstTwice_00303c80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, g.arg(1));
    g.write32(g.arg(0) + 8, g.arg(2));
    g.write32(g.arg(0), g.arg(1));
    g.ret_copy_of_arg(0);
}

// Initialiser: +0 = a1, +4 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Pair_InitFirst_003607b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), g.arg(1));
    g.write32(g.arg(0) + 4, 0);
    g.ret_copy_of_arg(0);
}

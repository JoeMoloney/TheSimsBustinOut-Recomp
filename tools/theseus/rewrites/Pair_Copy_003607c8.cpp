// Copies the two words at a1 to a0 (first, then second). Returns a0. 2 call sites.
#include "theseus.h"

void Pair_Copy_003607c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), g.read32(g.arg(1)));
    g.write32(g.arg(0) + 4, g.read32(g.arg(1) + 4));
    g.ret_copy_of_arg(0);
}

// Stores a1 at a0+0 and a0+4. 2 call sites.
#include "theseus.h"

void Pair_SetBoth_00275e98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), g.arg(1));
    g.write32(g.arg(0) + 4, g.arg(1));
    g.ret_void();
}

// Stores a2 at a0+4 and a1 at a0+0. 1 call site.
#include "theseus.h"

void Pair_Set_002be680(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, g.arg(2));
    g.write32(g.arg(0), g.arg(1));
    g.ret_void();
}

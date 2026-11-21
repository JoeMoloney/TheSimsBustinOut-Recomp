// Zeroes the 3 words at a0 (+8, +0, +4). 1 call site.
#include "theseus.h"

void Words3_Zero_002afa50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 8, 0);
    g.write32(g.arg(0), 0);
    g.write32(g.arg(0) + 4, 0);
    g.ret_void();
}

// *a1 = 0, *a2 = 1; returns 1. 1 call site.
#include "theseus.h"

void Outputs_ZeroOne_00370c98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(1), 0);
    g.write32(g.arg(2), 1);
    g.ret(1);
}

// 64-bit multiply (libgcc __muldi3): a0 * a1 modulo 2^64, built from 32-bit products as the
// original does; returns all 64 bits of v0. 3 call sites.
#include "theseus.h"

void MulDi3_0022b820(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret64(g.arg64(0) * g.arg64(1));
}

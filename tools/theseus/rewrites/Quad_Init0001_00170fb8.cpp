// Initialiser: +8 = 0, +4 = 0, +0xC = 1, +0 = 0. Returns a0 (v1 = a0 + 4, a1 = 1). 1 call site.
#include "theseus.h"

void Quad_Init0001_00170fb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 0);
    g.write32(p + 4, 0);
    g.write32(p + 0xC, 1);
    g.write32(p, 0);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(p + 4));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

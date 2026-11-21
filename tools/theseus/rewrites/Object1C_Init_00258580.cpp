// Initialiser: +8 = 1, +0x10 = 0, +0xC = 0, +0 = 1, +0x18 = 0, +0x14 = 0. Returns a0 (v1 = 1). 1 call site.
#include "theseus.h"

void Object1C_Init_00258580(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 1);
    g.write32(p + 0x10, 0);
    g.write32(p + 0xC, 0);
    g.write32(p, 1);
    g.write32(p + 0x18, 0);
    g.write32(p + 0x14, 0);
    SET_GPR_S32(ctx, 3, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

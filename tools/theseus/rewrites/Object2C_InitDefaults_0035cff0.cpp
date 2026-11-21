// Initialiser: +0 = 0x400, +0xC = 4, +0x18 = 0x3C, +0x1C = 2; +4, +8, +0x10, +0x14, +0x20, +0x24, +0x28 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object2C_InitDefaults_0035cff0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0x400);
    g.write32(p + 0xC, 4);
    g.write32(p + 0x18, 0x3C);
    g.write32(p + 0x1C, 2);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.write32(p + 0x10, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x20, 0);
    g.write32(p + 0x24, 0);
    g.write32(p + 0x28, 0);
    SET_GPR_S32(ctx, 3, 4);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 0x3C);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 2);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

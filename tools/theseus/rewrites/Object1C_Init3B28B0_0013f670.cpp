// Initialiser: +0x18 = 0x3B28B0, +0xC = 0, +8 = 0, +0x10 = 1, +0 = 2 as a 64-bit store (+4 = 0), +0x14 = 0. Returns a0 (v1 = 2, a1 = a0 + 8, a2 = 1). 1 call site.
#include "theseus.h"

void Object1C_Init3B28B0_0013f670(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x18, 0x3B28B0);
    g.write32(p + 0xC, 0);
    g.write32(p + 8, 0);
    g.write32(p + 0x10, 1);
    g.write64(p, 2);
    g.write32(p + 0x14, 0);
    SET_GPR_S32(ctx, 3, 2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, static_cast<int32_t>(p + 8));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

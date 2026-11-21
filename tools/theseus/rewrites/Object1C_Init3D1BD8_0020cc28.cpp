// Initialiser: +8 = -1, +0 = a1, +0xC = 1, +0x10 = 0x3D1BD8, +4/+0x14/+0x18 = 0. Returns a0 (v1 = 0x3D1BD8, a2 = 1). 1 call site. (The map joined a setter after the `jr $ra`.)
#include "theseus.h"

void Object1C_Init3D1BD8_0020cc28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 0xFFFFFFFFu);
    g.write32(p, g.arg(1));
    g.write32(p + 0xC, 1);
    g.write32(p + 0x10, 0x3D1BD8);
    g.write32(p + 4, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x18, 0);
    SET_GPR_S32(ctx, 3, 0x3D1BD8);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

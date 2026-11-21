// Constructor: +0 = 0x3E6088 (vtable), +4 = -1, +0xC = 8, +0x10 = 7, +0x14 = 2. Returns a0 (v1/a1/a2/a3 = the constants). 1 call site.
#include "theseus.h"

void Object18_Init3E6088_002c4720(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0x3E6088);
    g.write32(p + 4, 0xFFFFFFFFu);
    g.write32(p + 0xC, 8);
    g.write32(p + 0x10, 7);
    g.write32(p + 0x14, 2);
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 8);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 7);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, 2);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

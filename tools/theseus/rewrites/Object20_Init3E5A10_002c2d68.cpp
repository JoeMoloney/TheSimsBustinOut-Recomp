// Constructor: +0x1C = 0x3E5A10 (vtable), +8 = 3, +0x18 = 1, +0xC..+0x17 = 0 (through a stack temporary), +0 = +4 = -1 (the original first stores 0x40 there; the -1 stores follow with nothing in between that reads them). Returns a0. 1 call site.
#include "theseus.h"

void Object20_Init3E5A10_002c2d68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x1C, 0x3E5A10);
    g.write32(p + 8, 3);
    g.write32(p + 0x18, 1);
    g.write64(p + 0xC, 0);
    g.write32(p + 0x14, 0);
    g.write32(p, 0xFFFFFFFFu);
    g.write32(p + 4, 0xFFFFFFFFu);
    SET_GPR_U64(ctx, 3, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, 3);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 8, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

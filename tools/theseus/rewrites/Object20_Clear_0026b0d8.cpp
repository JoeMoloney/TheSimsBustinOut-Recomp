// Zeroes the 0x20-byte object at a0 (the original zeroes +0xC..+0x17 through a stack temporary, copies them to +0..+0xB, then zeroes +0x1C and +0x18). Returns a0. 1 call site.
#include "theseus.h"

void Object20_Clear_0026b0d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write64(p + 0xC, 0);
    g.write32(p + 0x14, 0);
    g.write64(p, 0);
    g.write32(p + 8, 0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x18, 0);
    SET_GPR_U64(ctx, 3, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

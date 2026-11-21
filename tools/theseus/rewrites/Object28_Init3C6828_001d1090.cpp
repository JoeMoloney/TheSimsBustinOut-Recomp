// Initialiser: +0xC = 0x3C6828, +8 = 0x3C6848, +0x10..+0x1C = 0, halfword +0x24 = 0, +0 = 1. Returns a0. (The original also writes two words of a stack temporary.) 1 call site.
#include "theseus.h"

void Object28_Init3C6828_001d1090(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, 0x3C6828);
    g.write32(p + 8, 0x3C6848);
    g.write32(p + 0x10, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x18, 0);
    g.write32(p + 0x1C, 0);
    g.write16(p + 0x24, 0);
    g.write32(p, 1);
    SET_GPR_S32(ctx, 5, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 3, 0x3C6848);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

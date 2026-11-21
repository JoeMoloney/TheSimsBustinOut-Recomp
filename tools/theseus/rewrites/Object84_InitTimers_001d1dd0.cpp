// Initialiser: +0x58 = 10; +0x48, +0x54, +0x50, +0x5C = 0; the 9 words +0x80 down to +0x60 = 0. Returns a0 + 0x5C (what the loop leaves; v1 = -1). 1 call site.
#include "theseus.h"

void Object84_InitTimers_001d1dd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x48, 0);
    g.write32(p + 0x58, 10);
    g.write32(p + 0x54, 0);
    g.write32(p + 0x50, 0);
    g.write32(p + 0x5C, 0);
    for (int32_t off = 0x80; off >= 0x60; off -= 4)
        g.write32(p + static_cast<uint32_t>(off), 0);
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(p + 0x5C);
}

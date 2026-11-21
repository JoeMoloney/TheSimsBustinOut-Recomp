// Pushes a0 onto the list at +8 of the block ([[0x3F91A0] + 0x5C] + 0x110); returns the block (v1 = the old head). 1 call site.
#include "theseus.h"

void Pool3F91A0_Push110_003675d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t block = g.read32(g.read32(0x3F91A0) + 0x5C) + 0x110;
    const uint32_t old = g.read32(block + 8);
    g.write32(g.arg(0), old);
    g.write32(block + 8, g.arg(0));
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(old));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(block);
}

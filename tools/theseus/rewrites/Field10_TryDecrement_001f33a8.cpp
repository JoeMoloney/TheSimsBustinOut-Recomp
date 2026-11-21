// Decrements the word at a0+0x10 when it is nonzero; returns 1 if it did, else 0. 1 call site.
#include "theseus.h"

void Field10_TryDecrement_001f33a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x10, value = g.read32(field);
    if (value == 0)
    {
        g.ret(0);
        return;
    }
    g.write32(field, value - 1);
    SET_GPR_S32(ctx, 3, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(1);
}

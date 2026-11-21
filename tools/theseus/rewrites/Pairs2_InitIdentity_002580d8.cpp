// Initialises two index pairs and flags: +0 = +4 = 0, +8 = +0xC = 1, +0x10 = +0x14 = 1. Returns 1. 1 call site.
#include "theseus.h"

void Pairs2_InitIdentity_002580d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    for (int32_t i = 0; i < 2; ++i)
    {
        g.write32(p + 8 * i, static_cast<uint32_t>(i));
        g.write32(p + 8 * i + 4, static_cast<uint32_t>(i));
        g.write32(p + 0x10 + 4 * i, 1);
    }
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(p + 0x10));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, static_cast<int32_t>(p + 0x18));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(1);
}

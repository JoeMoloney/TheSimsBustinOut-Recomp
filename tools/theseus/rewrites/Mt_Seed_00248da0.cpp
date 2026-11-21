// Seeds the game'\''s Mersenne Twister (the old sgenrand): state index 0x3A7728 = 0, mt[0] = a0 | 1, mt[i] = mt[i - 1] * 69069 for i = 1..623, state at 0x44B0B0. Returns 69069 (v0 holds the multiplier; v1 = the end of the state, a0 = the last value, a1 = 0). 1 call site.
#include "theseus.h"

void Mt_Seed_00248da0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t x = g.arg(0) | 1;
    g.write32(0x3A7728, 0);
    g.write32(0x44B0B0, x);
    for (uint32_t i = 1; i < 624; ++i)
    {
        x *= 0x10DCDu;
        g.write32(0x44B0B0 + 4 * i, x);
    }
    SET_GPR_S32(ctx, 3, 0x44B0B0 + 4 * 624);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, static_cast<int32_t>(x));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x10DCD);
}

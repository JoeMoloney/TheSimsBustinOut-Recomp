// isnan for the double in a0 (64-bit register): 1 if the exponent is all ones and the
// mantissa is nonzero, computed with integer operations as the original does (3 call sites).
#include "theseus.h"

void Double_IsNaN_002312b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t bits = g.arg64(0);
    const uint32_t high = static_cast<uint32_t>(bits >> 32) & 0x7FFFFFFFu;
    const uint32_t low_nonzero = static_cast<uint32_t>(bits) != 0 ? 1 : 0;  // gate: equivalent 2: any low bit gives the same result
    g.ret((0x7FF00000u - (high | low_nonzero)) >> 31);
}

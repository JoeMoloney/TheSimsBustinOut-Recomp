// Initialiser: +0 = 0, +0x44 = 0x3B7CD0, and in each of 4 groups of 16 bytes from +4 the first two words = 0 (+8 then +4, +0x18, +0x14, ...). Returns a0. 1 call site.
#include "theseus.h"

void Object48_Init3B7CD0_00167f40(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0);
    g.write32(p + 0x44, 0x3B7CD0);
    for (uint32_t grp = 0; grp < 4; ++grp)
    {
        g.write32(p + 8 + 16 * grp, 0);
        g.write32(p + 4 + 16 * grp, 0);
    }
    SET_GPR_S32(ctx, 3, 4);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 4);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, static_cast<int32_t>(p + 4));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

// Zeroes the 13 words a0+0..+0x30 (two passes over an array of 6 pairs, as the original orders them). Returns a0 (a1 = -1, v1 = a0 + 0x24). 1 call site.
#include "theseus.h"

void Object34_Clear_001f3590(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0);
    for (uint32_t i = 0; i < 2; ++i)
    {
        const uint32_t q = p + 0x1C + 4 * i;
        g.write32(q + 0x10, 0);
        g.write32(q + 8, 0);
        g.write32(q - 0x18, 0);
        g.write32(q - 0x10, 0);
        g.write32(q - 8, 0);
        g.write32(q, 0);
    }
    SET_GPR_S32(ctx, 5, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(p + 0x24));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

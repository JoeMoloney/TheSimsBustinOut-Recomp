// Resets two slot tables of a0: count +0x4500 = 0 and the first word of the 64 entries of 0x114 bytes from +0 (last first); count +0x4504 = 0 and the first word of the 32 entries of 0x110 bytes from +0x4614 (last first). Returns a0. 1 call site. Pairs with Slots114_Claim_00231f90.
#include "theseus.h"

void Slots_ResetAll_00231898(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x4500, 0);
    for (int32_t i = 63; i >= 0; --i)
        g.write32(p + 0x114 * static_cast<uint32_t>(i), 0);
    g.write32(p + 0x4504, 0);
    for (int32_t i = 31; i >= 0; --i)
        g.write32(p + 0x4614 + 0x110 * static_cast<uint32_t>(i), 0);
    SET_GPR_S32(ctx, 5, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(p + 0x4614 - 0x110));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

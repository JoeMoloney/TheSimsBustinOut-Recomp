// Sets (a2 != 0, whole register) or clears bit a1 of the bit array whose word pointer is *a0. Returns the new word. 1 call site. (The map joined another function'\''s epilogue after the `jr $ra`.)
#include "theseus.h"

void BitArray_SetOrClear_002ac490(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t word = g.read32(g.arg(0)) + ((g.arg(1) >> 5) << 2);
    const uint32_t bit = 1u << (g.arg(1) & 31);
    uint32_t value;
    if (g.arg64(2) != 0)
    {
        value = g.read32(word) | bit;
        SET_GPR_S32(ctx, 3, static_cast<int32_t>((g.arg(1) >> 5) << 2));  // scratch register for callers (not ABI-compared); gate: equivalent
    }
    else
    {
        value = g.read32(word) & ~bit;
        SET_GPR_S32(ctx, 3, static_cast<int32_t>(~bit));  // scratch register for callers (not ABI-compared); gate: equivalent
    }
    g.write32(word, value);
    SET_GPR_S32(ctx, 4, static_cast<int32_t>(bit));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, static_cast<int32_t>(g.arg(1) & 31));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, static_cast<int32_t>(word));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(value);
}

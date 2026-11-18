// Value of slot i in a two-slot array of {key, value} pairs, or -1 for i >= 2 (72 callers).
// Replaces FUN_00258268 (array = a0, i = a1). The bound check uses the whole 64-bit register.
#include "theseus.h"

void TwoSlot_Value_00258268(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t i = g.arg64(1);
    g.ret(i < 2 ? g.read32(g.arg(0) + static_cast<uint32_t>(i) * 8 + 4) : 0xFFFFFFFFu);
}

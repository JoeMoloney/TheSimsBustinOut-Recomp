// (word +0x20) + (word +0x2B8) of a0 if below 12 (signed); else minus 12 when word +0x2C4 is
// set, or 11 (4 call sites).
#include "theseus.h"

void Wrap12Sum_20_2B8_0035ce88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const int32_t sum = static_cast<int32_t>(g.read32(a + 0x20) + g.read32(a + 0x2B8));
    if (sum < 12)
        g.ret(static_cast<uint32_t>(sum));
    else
        g.ret(g.read32(a + 0x2C4) != 0 ? static_cast<uint32_t>(sum - 12) : 11);
}

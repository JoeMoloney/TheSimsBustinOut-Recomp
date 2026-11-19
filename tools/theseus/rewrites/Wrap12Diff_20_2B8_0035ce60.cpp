// (word +0x20) - (word +0x2B8) of a0; if negative: plus 12 when word +0x2C4 is set, else 0
// (month-style wrap; 4 call sites).
#include "theseus.h"

void Wrap12Diff_20_2B8_0035ce60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const int32_t diff = static_cast<int32_t>(g.read32(a + 0x20) - g.read32(a + 0x2B8));
    if (diff >= 0)
        g.ret(static_cast<uint32_t>(diff));
    else
        g.ret(g.read32(a + 0x2C4) != 0 ? static_cast<uint32_t>(diff + 12) : 0);
}

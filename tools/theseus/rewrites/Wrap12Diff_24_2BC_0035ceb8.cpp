// (word +0x24) - (word +0x2BC) of a0; if negative: plus 12 when word +0x2C0 is set, else 0
// (4 call sites).
#include "theseus.h"

void Wrap12Diff_24_2BC_0035ceb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const int32_t diff = static_cast<int32_t>(g.read32(a + 0x24) - g.read32(a + 0x2BC));
    if (diff >= 0)
        g.ret(static_cast<uint32_t>(diff));
    else
        g.ret(g.read32(a + 0x2C0) != 0 ? static_cast<uint32_t>(diff + 12) : 0);
}

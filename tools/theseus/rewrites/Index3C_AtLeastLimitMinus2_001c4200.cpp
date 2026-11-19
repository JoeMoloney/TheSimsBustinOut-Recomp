// 0 if the signed index at +0x3C of a0 is negative; otherwise whether it is at least
// (word +0xAC of the object at +0x6C) - 2, signed (3 call sites).
#include "theseus.h"

void Index3C_AtLeastLimitMinus2_001c4200(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const int32_t index = static_cast<int32_t>(g.read32(a + 0x3C));
    if (index < 0)
    {
        g.ret(0);
        return;
    }
    const int32_t limit = static_cast<int32_t>(g.read32(g.read32(a + 0x6C) + 0xAC) - 2);
    g.ret(index < limit ? 0 : 1);
}

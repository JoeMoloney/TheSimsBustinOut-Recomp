// Element a1 of the vector of words from +0x8 (begin) to +0xC (end) of a0, or 0 if a1 is
// negative or not below the count (5 call sites).
#include "theseus.h"

void Vector4_At_00347600(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.sarg64(1) < 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t a = g.arg(0);
    const uint32_t begin = g.read32(a + 8);
    const int32_t count = static_cast<int32_t>(g.read32(a + 0xC) - begin) >> 2;
    if (g.arg64(1) < static_cast<uint64_t>(static_cast<int64_t>(count)))
        g.ret(g.read32(begin + (g.arg(1) << 2)));
    else
        g.ret(0);
}

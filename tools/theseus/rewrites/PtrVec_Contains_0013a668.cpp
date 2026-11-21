// 1 if the word vector [a0+0, a0+4) contains a1 (all 64 bits), else 0. 1 call site.
#include "theseus.h"

void PtrVec_Contains_0013a668(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t begin = g.read32(g.arg(0));
    const int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 4) - begin) >> 2;
    const int64_t key = g.sarg64(1);
    for (int32_t i = 0; i < count; ++i)
        if (static_cast<int32_t>(g.read32(g.read32(g.arg(0)) + 4 * i)) == key)
        {
            g.ret(1);
            return;
        }
    g.ret(0);
}

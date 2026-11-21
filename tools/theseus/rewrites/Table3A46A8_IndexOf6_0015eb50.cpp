// Index of a0 (all 64 bits) among the 6 words at 0x3A46A8, or -1. 1 call site.
#include "theseus.h"

void Table3A46A8_IndexOf6_0015eb50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(0);
    for (int32_t i = 0; i < 6; ++i)
        if (static_cast<int32_t>(g.read32(0x003A46A8 + 4 * i)) == key)
        {
            g.ret(i);
            return;
        }
    g.ret(0xFFFFFFFF);
}

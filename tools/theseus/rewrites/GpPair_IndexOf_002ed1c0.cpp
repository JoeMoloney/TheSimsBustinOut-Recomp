// Index (0 or 1) of a0 (all 64 bits) in the two words at gp-0x7698, or -1. 1 call site.
#include "theseus.h"

void GpPair_IndexOf_002ed1c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(0);
    for (int32_t i = 0; i < 2; ++i)
        if (static_cast<int32_t>(g.read32(g.gp() - 0x7698 + 4 * i)) == key)
        {
            g.ret(i);
            return;
        }
    g.ret(0xFFFFFFFF);
}

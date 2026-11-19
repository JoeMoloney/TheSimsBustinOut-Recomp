// 0 if the object at +0x3F38 of a0 is null or appears in the chain starting at a1 (next pointer
// at +0x48); otherwise 1. The first link is compared on all 64 bits of a1. 3 call sites.
#include "theseus.h"

void Chain48_NotContains_0023d158(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t target = static_cast<int32_t>(g.read32(g.arg(0) + 0x3F38));
    if (target == 0)
    {
        g.ret(0);
        return;
    }
    for (int64_t link = g.sarg64(1); link != 0; link = static_cast<int32_t>(g.read32(static_cast<uint32_t>(link) + 0x48)))
        if (link == target)
        {
            g.ret(0);
            return;
        }
    g.ret(1);
}

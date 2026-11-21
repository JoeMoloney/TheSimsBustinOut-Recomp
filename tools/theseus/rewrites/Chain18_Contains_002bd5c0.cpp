// 1 if a1 is a0 or appears in the chain from a0 (next at +0x18), else 0. All 64 bits of a0 and
// a1 are compared for the first link. 1 call site.
#include "theseus.h"

void Chain18_Contains_002bd5c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t wanted = g.sarg64(1);
    for (int64_t link = g.sarg64(0);; )
    {
        if (link == wanted)
        {
            g.ret(1);
            return;
        }
        link = static_cast<int32_t>(g.read32(static_cast<uint32_t>(link) + 0x18));
        if (link == 0)
            break;
    }
    g.ret(0);
}

// Searches the vector of object pointers from +0 (begin) to +4 (end) of a0 for the first object
// whose word +0x14 equals a1 (all 64 bits, as `bne`); returns it or 0. 20 call sites.
#include "theseus.h"

void PtrVector_FindByField14_0021c970(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 4);
    for (uint32_t it = g.read32(g.arg(0)); it != end; it += 4)
    {
        const uint32_t obj = g.read32(it);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj + 0x14))) == g.sarg64(1))
        {
            g.ret(obj);
            return;
        }
    }
    g.ret(0);
}

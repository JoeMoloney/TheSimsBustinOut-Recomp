// The first object in the vector of object pointers from +0x24 (begin) to +0x28 (end) of a0
// whose word +0 equals a1 (all 64 bits); 0 if none (3 call sites).
#include "theseus.h"

void PtrVector_FindByField0_0021c570(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 0x28);
    for (uint32_t it = g.read32(g.arg(0) + 0x24); it != end; it += 4)
    {
        const uint32_t obj = g.read32(it);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj))) == g.sarg64(1))
        {
            g.ret(obj);
            return;
        }
    }
    g.ret(0);
}

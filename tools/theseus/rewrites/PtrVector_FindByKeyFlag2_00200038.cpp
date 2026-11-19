// Searches the vector of object pointers from +8 (begin) to +0xC (end) of a0 for the first
// object whose word +0 equals a1 (all 64 bits) and whose word +4 has bit 2 set; returns it or 0
// (5 call sites).
#include "theseus.h"

void PtrVector_FindByKeyFlag2_00200038(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 0xC);
    for (uint32_t it = g.read32(g.arg(0) + 8); it != end; it += 4)
    {
        const uint32_t obj = g.read32(it);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj))) != g.sarg64(1))
            continue;
        const uint32_t flags = g.read32(obj + 4);  // gate: equivalent read16: only bit 2 is used
        if ((flags >> 2) & 1)
        {
            g.ret(obj);
            return;
        }
    }
    g.ret(0);
}

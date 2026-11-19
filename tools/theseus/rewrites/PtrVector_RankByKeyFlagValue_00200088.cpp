// Among the objects in the vector from +8 (begin) to +0xC (end) of a0 whose word +0 equals a1
// and whose word +4 has bit 0 set, the position (from 0) of the first one whose word +8 equals
// a2; -1 if there is none. Compares are on all 64 bits. 3 call sites.
#include "theseus.h"

void PtrVector_RankByKeyFlagValue_00200088(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 0xC);
    uint32_t rank = 0;
    for (uint32_t it = g.read32(g.arg(0) + 8); it != end; it += 4)
    {
        const uint32_t obj = g.read32(it);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj))) != g.sarg64(1))
            continue;
        const uint32_t flags = g.read32(obj + 4);  // gate: equivalent read16: only bit 0 is used
        if (!(flags & 1))
            continue;
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj + 8))) == g.sarg64(2))
        {
            g.ret(rank);
            return;
        }
        ++rank;
    }
    g.ret(0xFFFFFFFFu);
}

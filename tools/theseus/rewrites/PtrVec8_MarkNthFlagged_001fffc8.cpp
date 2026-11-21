// Walks the vector of object pointers at a0+8 (begin +8, end +0xC re-read each step). For each
// object whose key (+0) equals a1: clears bit 2 of its flags (+4); among those with flag bit 0
// set, the a2-th (counting from 0) gets bit 2 set instead. Returns the end pointer read last.
// 5 call sites.
#include "theseus.h"

void PtrVec8_MarkNthFlagged_001fffc8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t vec = g.arg(0) + 8;
    const int64_t key = g.sarg64(1), nth = g.sarg64(2);  // compared as whole registers
    int64_t seen = 0;
    uint32_t it = g.read32(vec), end = g.read32(vec + 4);
    if (it != end)
    {
        do
        {
            const uint32_t obj = g.read32(it);
            if (static_cast<int32_t>(g.read32(obj)) == key)
            {
                const uint32_t flags = g.read32(obj + 4), cleared = flags & ~4u;
                g.write32(obj + 4, cleared);  // gate: equivalent write16: clearing bit 2 leaves the upper half as it was
                if (flags & 1)
                {
                    if (seen == nth)
                        g.write32(obj + 4, cleared | 4);  // gate: equivalent write16,5: upper half as it was; bit 0 is already set on this path
                    ++seen;
                }
            }
            end = g.read32(vec + 4);
            it += 4;
        } while (it != end);
    }
    g.ret(end);
}

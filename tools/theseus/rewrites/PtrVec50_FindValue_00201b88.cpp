// Searches the word vector [a0+0x50, a0+0x54) for a1 (all 64 bits); returns the matching word
// (that is, a1) if found, else 0. 1 call site.
#include "theseus.h"

void PtrVec50_FindValue_00201b88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    const uint32_t end = g.read32(g.arg(0) + 0x54);
    for (uint32_t p = g.read32(g.arg(0) + 0x50); p != end; p += 4)
    {
        const uint32_t value = g.read32(p);
        if (static_cast<int32_t>(value) == key)
        {
            g.ret(value);
            return;
        }
    }
    g.ret(0);
}

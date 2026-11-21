// Searches the pointer array [a0+0x384, a0+0x388) for the first non-null object whose field +8
// equals a1 (all 64 bits of a1); returns it or 0. 2 call sites.
#include "theseus.h"

void PtrArray384_FindByField8_002e0c20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 0x388);
    const int64_t key = g.sarg64(1);
    for (uint32_t p = g.read32(g.arg(0) + 0x384); p != end; p += 4)
    {
        const uint32_t object = g.read32(p);
        if (object != 0 && static_cast<int32_t>(g.read32(object + 8)) == key)
        {
            g.ret(object);
            return;
        }
    }
    g.ret(0);
}

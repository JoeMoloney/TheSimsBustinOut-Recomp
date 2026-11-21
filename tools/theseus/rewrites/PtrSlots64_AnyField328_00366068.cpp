// 1 if any of the 64 pointer slots at a0 is a non-null object whose field +0x328 equals a1 (all
// 64 bits of a1), else 0. 2 call sites.
#include "theseus.h"

void PtrSlots64_AnyField328_00366068(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    for (uint32_t i = 0; i < 64; ++i)
    {
        const uint32_t object = g.read32(g.arg(0) + 4 * i);
        if (object != 0 && static_cast<int32_t>(g.read32(object + 0x328)) == key)
        {
            g.ret(1);
            return;
        }
    }
    g.ret(0);
}

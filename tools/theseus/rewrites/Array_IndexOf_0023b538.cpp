// Index of a1 (all 64 bits) in the word array [a0+0x10] of [a0+0xC] elements (signed), or -1. 1 call site.
#include "theseus.h"

void Array_IndexOf_0023b538(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 0xC));
    const uint32_t array = g.read32(g.arg(0) + 0x10);
    const int64_t key = g.sarg64(1);
    for (int32_t i = 0; i < count; ++i)
        if (static_cast<int32_t>(g.read32(array + 4 * i)) == key)
        {
            g.ret(i);
            return;
        }
    g.ret(0xFFFFFFFF);
}

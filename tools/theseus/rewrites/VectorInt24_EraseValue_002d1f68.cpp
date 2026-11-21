// Erases the first element equal to a1 (compared as a whole register) from the vector<int> at a0+0x24 (begin +0x24, end +0x28): the later elements move down one, end -= 4. Returns the new end, or the end when not found. 2 call sites.
#include "theseus.h"

void VectorInt24_EraseValue_002d1f68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t vec = g.arg(0) + 0x24;
    const int64_t value = g.sarg64(1);
    uint32_t it = g.read32(vec), end = g.read32(vec + 4);
    if (it == end)
    {
        g.ret(end);
        return;
    }
    while (static_cast<int32_t>(g.read32(it)) != value)
    {
        end = g.read32(vec + 4);
        it += 4;
        if (it == end)
        {
            g.ret(end);
            return;
        }
    }
    uint32_t src = it + 4;
    const uint32_t last = g.read32(vec + 4);
    if (src != last)
    {
        uint32_t dst = it;
        for (int32_t n = static_cast<int32_t>(last - src) >> 2; n > 0; --n, src += 4, dst += 4)
            g.write32(dst, g.read32(src));
    }
    const uint32_t new_end = g.read32(vec + 4) - 4;
    g.write32(vec + 4, new_end);
    g.ret(new_end);
}

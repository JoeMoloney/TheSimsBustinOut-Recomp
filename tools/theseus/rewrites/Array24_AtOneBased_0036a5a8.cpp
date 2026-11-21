// Vector of 24-byte elements (begin a0+0, end a0+4): returns the word at +16 of element a1-1
// (1-based index), or 0 if a1 <= 0 or a1 > size (signed, all 64 bits of a1). The size is computed
// as the compiler does it: (end - begin) * 0xAAAAAAAB >> 3 (exact division by 24). 2 call sites.
#include "theseus.h"

void Array24_AtOneBased_0036a5a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t index = g.sarg64(1);
    if (index <= 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t begin = g.read32(g.arg(0));
    const uint32_t bytes = g.read32(g.arg(0) + 4) - begin;
    const int32_t size = static_cast<int32_t>(bytes * 0xAAAAAAABu) >> 3;
    if (size < index)
    {
        g.ret(0);
        return;
    }
    g.ret(g.read32(g.arg(1) * 24 + begin - 8));
}

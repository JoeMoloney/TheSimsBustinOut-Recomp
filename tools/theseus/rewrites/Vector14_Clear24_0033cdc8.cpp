// Clears the vector at a0+0x24 (0x14-byte elements, nothing to destroy: end = begin; the original walks it with an empty loop). v0 = begin when it was empty, else end + 0x14 (what the walk leaves). 1 call site.
#include "theseus.h"

void Vector14_Clear24_0033cdc8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t begin = g.read32(g.arg(0) + 0x24), end = g.read32(g.arg(0) + 0x28);
    g.write32(g.arg(0) + 0x28, begin);
    g.ret(begin == end ? begin : end + 0x14);
}

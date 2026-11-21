// Fills the 10 words a0+0x1C..+0x40 (in pairs, high word first) with the global 0x3F9030, then +0x48 = 0, +0x44 = 0. Returns a0 + 0x44 (the loop pointer). 3 call sites.
#include "theseus.h"

void Words10_FillFrom3F9030_002d3b20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(0x3F9030);
    uint32_t p = g.arg(0) + 0x1C;
    for (int32_t i = 4; i >= 0; --i, p += 8)
    {
        g.write32(p + 4, value);
        g.write32(p, value);
    }
    g.write32(g.arg(0) + 0x48, 0);
    g.write32(g.arg(0) + 0x44, 0);
    g.ret(p);
}

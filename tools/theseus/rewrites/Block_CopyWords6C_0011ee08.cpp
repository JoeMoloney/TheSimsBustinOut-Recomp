// Copies block a2 (1 KB each, from a0+0x6C) to the buffer at [a1+0x24]: 0x100 words when [a1+0x3C] == 0, else 0x10 words. v0 = 0 (the last loop test). 3 call sites.
#include "theseus.h"

void Block_CopyWords6C_0011ee08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t info = g.arg(1);
    const uint32_t count = g.read32(info + 0x3C) == 0 ? 0x100 : 0x10;
    uint32_t dst = g.read32(info + 0x24);
    uint32_t src = g.arg(0) + 0x6C + (g.arg(2) << 10);
    for (uint32_t i = 0; i < count; ++i, src += 4, dst += 4)
        g.write32(dst, g.read32(src));
    g.ret(0);
}

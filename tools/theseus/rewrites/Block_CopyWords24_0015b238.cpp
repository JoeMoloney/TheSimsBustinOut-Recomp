// Copies 0x100 words (0x10 when a1+0x3C is nonzero) from a2 to the buffer at [a1+0x24]. v0 = 0 (the last loop test). 1 call site. (The same shape as Block_CopyWords6C_0011ee08.)
#include "theseus.h"

void Block_CopyWords24_0015b238(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t count = g.read32(g.arg(1) + 0x3C) == 0 ? 0x100 : 0x10;
    uint32_t dst = g.read32(g.arg(1) + 0x24), src = g.arg(2);
    for (uint32_t i = 0; i < count; ++i, src += 4, dst += 4)
        g.write32(dst, g.read32(src));
    g.ret(0);
}

// Copies 0x100 words (0x10 when a1+0x3C is nonzero) from the buffer [a1+0x24] to block a2 (1 KB each) at a0+0x6C. v0 = 0 (the last loop test). 1 call site. The reverse of Block_CopyWords6C_0011ee08.
#include "theseus.h"

void Block_CopyWordsTo6C_0011edb0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t count = g.read32(g.arg(1) + 0x3C) == 0 ? 0x100 : 0x10;
    uint32_t src = g.read32(g.arg(1) + 0x24);
    uint32_t dst = g.arg(0) + 0x6C + (g.arg(2) << 10);
    for (uint32_t i = 0; i < count; ++i, src += 4, dst += 4)
        g.write32(dst, g.read32(src));
    g.ret(0);
}

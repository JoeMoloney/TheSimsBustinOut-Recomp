// Copies a2 words from a1 to a0, last word first (safe when a0 > a1 overlap). Returns the last word copied (the first word), or a2 * 4 - 4 when nothing is copied. 3 call sites (3 identical copies).
// theseus: copies 0x2bac00, 0x2c3c98
#include "theseus.h"

void Words_CopyBackward_0017be78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t n = g.arg(2);
    uint32_t v0 = (n << 2) - 4;
    uint32_t dst = g.arg(0) + v0, src = g.arg(1) + v0;
    for (; n != 0; --n, dst -= 4, src -= 4)
    {
        v0 = g.read32(src);
        g.write32(dst, v0);
    }
    g.ret(v0);
}

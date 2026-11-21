// Copies a2 words from a1 to a0, one at a time in increasing order (so an overlap behaves as in the
// original); nothing for a2 == 0 (all 64 bits). v0 is left holding the last word copied. 9 call
// sites (5 identical copies).
// theseus: copies 0x17bd10, 0x25abc8, 0x2853d0, 0x2c3b30
#include "theseus.h"

void Words_Copy_001022c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(2) == 0)
    {
        g.ret_void();
        return;
    }
    // a do/while like the original: a count whose low 32 bits are 0 (upper half not) would copy 2^32 words
    uint32_t dst = g.arg(0), src = g.arg(1), left = g.arg(2), value;
    do
    {
        value = g.read32(src);
        g.write32(dst, value);
        src += 4;
        dst += 4;
    } while (--left != 0);
    SET_GPR_S32(ctx, 2, value);
    g.ret_void();
}

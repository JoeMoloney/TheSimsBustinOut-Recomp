// bstring: copy a NUL-terminated 8-bit string into a 16-bit buffer (each byte zero-extended).
// Replaces FUN_002c8fa0 (dst = a0, src = a1).
#include "theseus.h"

void bstring_WideFromNarrow_002c8fa0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1);
    while (g.read8(src) != 0)
    {
        g.write16(dst, g.read8(src));
        src += 1;
        dst += 2;
    }
    g.write16(dst, 0);
    g.ret(0);  // v0 still holds the terminator the loop stopped on
}

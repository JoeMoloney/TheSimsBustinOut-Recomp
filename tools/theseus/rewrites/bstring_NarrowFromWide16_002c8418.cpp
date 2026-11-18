// bstring: copy a NUL-terminated 16-bit string into an 8-bit buffer, keeping the low byte of
// each character. Replaces FUN_002c8418 (dst = a0, src = a1).
#include "theseus.h"

void bstring_NarrowFromWide16_002c8418(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1);
    while (g.read16(src) != 0)
    {
        g.write8(dst, g.read8(src));  // low byte first (little-endian) // gate: equivalent read16 here: write8 keeps only the low byte
        src += 2;
        dst += 1;
    }
    g.write8(dst, 0);
    g.ret(0);  // v0 still holds the terminator the loop stopped on
}

// bstring: copy a NUL-terminated 32-bit string into a 16-bit buffer, keeping the low 16 bits
// of each character. Replaces FUN_002c8f68 (dst = a0, src = a1).
#include "theseus.h"

void bstring_Wide16FromWide32_002c8f68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1);
    while (g.read32(src) != 0)
    {
        g.write16(dst, g.read16(src));  // gate: equivalent read32 here: write16 keeps only the low half
        src += 4;
        dst += 2;
    }
    g.write16(dst, 0);
    g.ret(0);  // v0 still holds the terminator the loop stopped on
}

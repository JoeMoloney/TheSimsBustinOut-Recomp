// Copies the 16-bit string at a0 to a 8-bit buffer a1 (low byte of each character) and
// terminates it; at most a2 - 1 characters (a2 == 1: none; a2 == 0: no limit). Returns the
// number of characters copied. 8 call sites.
#include "theseus.h"

void Wstr16_ToAsciiN_001cf900(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t src = g.arg(0), dst = g.arg(1), count = 0;
    const uint32_t limit = g.arg(2) - 1;
    if (g.read16(src) != 0 && limit != 0)
    {
        do
        {
            g.write8(dst, g.read8(src));  // gate: equivalent read16: write8 keeps only the low byte
            ++count;
            src += 2;
            ++dst;
        } while (g.read16(src) != 0 && count < limit);
    }
    g.write8(dst, 0);
    g.ret(count);
}

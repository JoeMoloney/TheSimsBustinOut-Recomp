// Copies the 8-bit string at a0 to a 16-bit buffer a1 (each character sign-extended) and
// terminates it; at most a2 - 1 characters (a2 == 1: none; a2 == 0: no limit). Returns the
// number of characters copied. 7 call sites. The reverse of Wstr16_ToAsciiN_001cf900.
#include "theseus.h"

void Str_ToWstr16N_001cf8a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t src = g.arg(0), dst = g.arg(1), count = 0;
    const uint32_t limit = g.arg(2) - 1;
    if (g.read8(src) != 0 && limit != 0)
    {
        do
        {
            g.write16(dst, static_cast<uint16_t>(g.sread8(src)));
            ++count;
            ++src;
            dst += 2;
        } while (g.read8(src) != 0 && count < limit);
    }
    g.write16(dst, 0);
    g.ret(count);
}

// Sets (a2 != 0) or clears (a2 == 0) bit a1 (mod 32) of the word at a0+0x8C. v0 = the mask
// (1 << a1, inverted when clearing). 6 call sites.
#include "theseus.h"

void Field8C_SetBit_00256f30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x8C, bit = 1u << (g.arg(1) & 31);
    if (g.arg64(2) != 0)
    {
        g.write32(field, g.read32(field) | bit);
        g.ret(bit);
    }
    else
    {
        g.write32(field, g.read32(field) & ~bit);
        g.ret(~bit);
    }
}

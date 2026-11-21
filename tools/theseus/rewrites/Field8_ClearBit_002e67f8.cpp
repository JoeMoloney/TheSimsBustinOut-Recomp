// Clears bit a1 (mod 32) of the word at a0+8; v0 = the inverted mask. 2 call sites. Pairs with Field8_SetBit_002e67e0.
#include "theseus.h"

void Field8_ClearBit_002e67f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 8, mask = ~(1u << (g.arg(1) & 31));
    g.write32(field, g.read32(field) & mask);
    g.ret(mask);
}

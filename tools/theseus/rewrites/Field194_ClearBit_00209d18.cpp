// Clears bit a1 (mod 32) of the word at a0+0x194. v0 = the inverted mask. 4 call sites.
#include "theseus.h"

void Field194_ClearBit_00209d18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x194, mask = ~(1u << (g.arg(1) & 31));
    g.write32(field, g.read32(field) & mask);
    g.ret(mask);
}

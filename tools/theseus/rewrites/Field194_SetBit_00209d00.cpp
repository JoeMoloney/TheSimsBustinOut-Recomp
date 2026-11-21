// Sets bit a1 (mod 32) of the word at a0+0x194; v0 = the mask. 2 call sites. Pairs with Field194_ClearBit_00209d18.
#include "theseus.h"

void Field194_SetBit_00209d00(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x194, bit = 1u << (g.arg(1) & 31);
    g.write32(field, g.read32(field) | bit);
    g.ret(bit);
}

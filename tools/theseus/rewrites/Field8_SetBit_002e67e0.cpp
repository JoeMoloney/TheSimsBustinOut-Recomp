// Sets bit a1 (mod 32) of the word at a0+8; v0 = the mask. 2 call sites.
#include "theseus.h"

void Field8_SetBit_002e67e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 8, bit = 1u << (g.arg(1) & 31);
    g.write32(field, g.read32(field) | bit);
    g.ret(bit);
}

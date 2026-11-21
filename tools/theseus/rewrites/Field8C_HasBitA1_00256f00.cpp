// 1 if bit (a1 & 31) of the word at a0+0x8C is set; else 0, with v0 cleared to all 128 bits
// (`movz v0, $zero`). 1 call site.
#include "theseus.h"

void Field8C_HasBitA1_00256f00(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0) + 0x8C) & (1u << (g.arg(1) & 31)))
        g.ret(1);
    else
        g.ret_with_upper(0, _mm_setzero_si128());
}

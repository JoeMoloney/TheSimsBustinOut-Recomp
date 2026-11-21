// 1 unless the word at a0+0x58 is -1. 1 call site.
#include "theseus.h"

void Field58_NotMinus1_00351548(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x58) != 0xFFFFFFFF ? 1 : 0);
}

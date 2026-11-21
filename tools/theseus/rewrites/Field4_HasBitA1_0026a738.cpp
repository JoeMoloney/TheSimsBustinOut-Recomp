// 1 if bit (a1 & 31) of the word at a0+4 is set, else 0. 1 call site.
#include "theseus.h"

void Field4_HasBitA1_0026a738(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((g.read32(g.arg(0) + 4) & (1u << (g.arg(1) & 31))) != 0 ? 1 : 0);
}

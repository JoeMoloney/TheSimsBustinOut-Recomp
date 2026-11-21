// Bit (a1 & 31) of the word at a0+8 (arithmetic shift, then & 1). 1 call site.
#include "theseus.h"

void Field8_BitA1_002e6818(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((static_cast<int32_t>(g.read32(g.arg(0) + 8)) >> (g.arg(1) & 31)) & 1);
}

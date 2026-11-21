// 1 if the word at a0+4 is 0, else 0. 1 call site.
#include "theseus.h"

void Field4_IsZero_00277dd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 4) == 0 ? 1 : 0);
}

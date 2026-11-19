// True if the word at a0 is nonzero (2 call sites).
#include "theseus.h"

void Field0_NonZero_00345168(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0)) != 0 ? 1 : 0);
}

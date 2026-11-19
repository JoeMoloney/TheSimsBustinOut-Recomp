// True if the word at +0xC of a0 is nonzero (5 call sites).
#include "theseus.h"

void Field0C_NonZero_0035d1b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0xC) != 0 ? 1 : 0);
}

// True if the word at +0x114 of the object in a0 is nonzero (12 call sites).
#include "theseus.h"

void Field114_NonZero_0036fd90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x114) != 0 ? 1 : 0);
}

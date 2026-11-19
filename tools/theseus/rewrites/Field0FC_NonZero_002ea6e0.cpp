// True if the word at +0xFC of a0 is nonzero (5 call sites).
#include "theseus.h"

void Field0FC_NonZero_002ea6e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0xFC) != 0 ? 1 : 0);
}

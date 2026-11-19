// True if the word at +0x194 of a0 is nonzero (2 call sites).
#include "theseus.h"

void Field194_NonZero_00209cf0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x194) != 0 ? 1 : 0);
}

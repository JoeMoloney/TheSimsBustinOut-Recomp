// The word at +0x88 of the object a0+4 points to. 1 call site.
#include "theseus.h"

void Field4_Field88_0021c088(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.arg(0) + 4) + 0x88));
}

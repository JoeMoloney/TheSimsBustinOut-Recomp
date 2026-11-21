// The word at +0 of the object at a0+0x34. 1 call site.
#include "theseus.h"

void Field34_Deref_0021c2f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.arg(0) + 0x34)));
}

// The word at +0 of the object at a0+0x14, or 0 if that is null. 1 call site.
#include "theseus.h"

void Field14_Deref0_0035bd50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.read32(g.arg(0) + 0x14);
    g.ret(p ? g.read32(p) : 0);
}

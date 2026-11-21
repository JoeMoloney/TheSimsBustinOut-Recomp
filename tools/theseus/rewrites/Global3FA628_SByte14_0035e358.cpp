// The signed byte at +0x14 of the object the global 0x3FA628 points to. 1 call site.
#include "theseus.h"

void Global3FA628_SByte14_0035e358(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.sread8(g.read32(0x003FA628) + 0x14));
}

// Zeroes +0x88 of the object [a0+4]; returns that object. 1 call site.
#include "theseus.h"

void Child4_ClearField88_0021c098(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 4);
    g.write32(obj + 0x88, 0);
    g.ret(obj);
}

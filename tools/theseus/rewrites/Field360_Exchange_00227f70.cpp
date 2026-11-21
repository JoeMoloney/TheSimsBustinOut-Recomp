// Stores a1 at a0+0x360 and returns the previous value. 11 call sites.
#include "theseus.h"

void Field360_Exchange_00227f70(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t old = g.read32(g.arg(0) + 0x360);
    g.write32(g.arg(0) + 0x360, g.arg(1));
    g.ret(old);
}

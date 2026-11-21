// Stores a1 at a0+0x1980 and ([a0+0x1984] - a1) at a0+0x1990; returns the difference. 1 call site.
#include "theseus.h"

void Field1980_SetGetRest_001a1d08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t total = g.read32(g.arg(0) + 0x1984);
    g.write32(g.arg(0) + 0x1980, g.arg(1));
    const uint32_t rest = total - g.arg(1);
    g.write32(g.arg(0) + 0x1990, rest);
    g.ret(rest);
}

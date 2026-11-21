// Stores a1 at a0+0x18C and returns the old word. 1 call site.
#include "theseus.h"

void Field18C_Exchange_001b41a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x18C, old = g.read32(field);
    g.write32(field, g.arg(1));
    g.ret(old);
}

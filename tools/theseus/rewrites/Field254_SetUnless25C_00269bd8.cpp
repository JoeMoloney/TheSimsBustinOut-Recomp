// Stores a1 at a0+0x254 when the word at a0+0x25C is 0; returns that word. 1 call site.
#include "theseus.h"

void Field254_SetUnless25C_00269bd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t guard = g.read32(g.arg(0) + 0x25C);
    if (guard == 0)
        g.write32(g.arg(0) + 0x254, g.arg(1));
    g.ret(guard);
}

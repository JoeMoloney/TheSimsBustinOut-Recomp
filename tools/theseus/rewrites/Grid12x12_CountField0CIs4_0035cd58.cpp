// Counts the objects in the 12 x 12 pointer grid at a0+0x58 whose word at +0xC is 4. 1 call site.
#include "theseus.h"

void Grid12x12_CountField0CIs4_0035cd58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t count = 0;
    for (uint32_t i = 0; i < 144; ++i)
        if (g.read32(g.read32(g.arg(0) + 0x58 + 4 * i) + 0xC) == 4)
            ++count;
    g.ret(count);
}

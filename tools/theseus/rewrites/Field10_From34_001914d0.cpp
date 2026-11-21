// Copies the word at a0+0x34 to a0+0x10 and returns it. 1 call site.
#include "theseus.h"

void Field10_From34_001914d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 0x34);
    g.write32(g.arg(0) + 0x10, value);
    g.ret(value);
}

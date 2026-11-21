// Copies the word at a0+8 to a0+0xC and returns it. 1 call site.
#include "theseus.h"

void FieldC_From8_003475f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 8);
    g.write32(g.arg(0) + 0xC, value);
    g.ret(value);
}

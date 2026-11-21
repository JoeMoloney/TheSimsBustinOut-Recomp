// Copies a0+0x5C to a0+0x60 and zeroes +0x4C, +0x54, +0x50, +0x58. Returns the copied word. 1 call site.
#include "theseus.h"

void Fields4C_60_Reset_001ae738(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0), value = g.read32(p + 0x5C);
    g.write32(p + 0x4C, 0);
    g.write32(p + 0x60, value);
    g.write32(p + 0x54, 0);
    g.write32(p + 0x50, 0);
    g.write32(p + 0x58, 0);
    g.ret(value);
}

// Zeroes the 6 words at a0 (+0x14, then +0..+0x10). 1 call site.
#include "theseus.h"

void Words6_Zero_0021c7b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x14, 0);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x10, 0);
    g.ret_void();
}

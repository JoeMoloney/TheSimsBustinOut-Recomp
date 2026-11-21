// Resets an object: +0x118, +0, +4, +0xC, +0x110, +0x114 = 0, halfword +0x10 = 0, +8 = 1.
// Returns 1. 4 call sites.
#include "theseus.h"

void Object_Reset118_001a8388(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x118, 0);
    g.write32(p + 8, 1);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.write32(p + 0xC, 0);
    g.write16(p + 0x10, 0);
    g.write32(p + 0x110, 0);
    g.write32(p + 0x114, 0);
    g.ret(1);
}

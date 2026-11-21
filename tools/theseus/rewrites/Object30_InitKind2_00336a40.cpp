// Initialiser: +0xC = 2; +0, +4, +8, +0x14..+0x20, +0x28, +0x2C = 0; halfword +0x10 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object30_InitKind2_00336a40(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0);
    g.write32(p + 0xC, 2);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.write16(p + 0x10, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x18, 0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x20, 0);
    g.write32(p + 0x28, 0);
    g.write32(p + 0x2C, 0);
    g.ret_copy_of_arg(0);
}

// Initialiser: +0 = a1, +4 = a2, +0xC = a3, halfword +0x10 = a4 (t0); +8, +0x14..+0x20,
// +0x28, +0x2C = 0. Returns a0. 4 call sites.
#include "theseus.h"

void Object30_Init_00336a08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 4, g.arg(2));
    g.write32(p + 0xC, g.arg(3));
    g.write16(p + 0x10, static_cast<uint16_t>(g.arg(4)));
    g.write32(p + 8, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x18, 0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x20, 0);
    g.write32(p + 0x28, 0);
    g.write32(p + 0x2C, 0);
    g.ret_copy_of_arg(0);
}

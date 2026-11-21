// Initialiser: +8 = 1, byte +0x38 = 0xFF, byte +0x16 = 10, +0 = 1, +4/+0xC/+0x10 = 0, bytes +0x14/+0x15 = 10, +0x17/+0x18 = 0. Returns -1. 2 call sites.
#include "theseus.h"

void Object3C_Init_00190f68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 1);
    g.write8(p + 0x38, 0xFF);
    g.write8(p + 0x16, 10);  // gate: equivalent write16: +0x17 is stored later
    g.write32(p, 1);
    g.write32(p + 4, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x10, 0);
    g.write8(p + 0x14, 10);  // gate: equivalent write16: +0x15 is stored next
    g.write8(p + 0x15, 10);
    g.write8(p + 0x17, 0);  // gate: equivalent write16: +0x18 is stored next
    g.write8(p + 0x18, 0);
    g.ret(0xFFFFFFFFu);
}

// Initialises a 0x1A-byte struct at a0: +0 = 0, +4 = 1, bytes +8..+0x11 = 0, +0x12..+0x14 = 1, +0x15 = 0,
// +0x16 = 1, +0x17..+0x19 = 0 (stores in the original's order); returns a0 (all 64 bits). 21 call sites.
#include "theseus.h"

void Struct1A_Init_002d1238(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write8(p + 0x16, 1);  // gate: equivalent write16: the next byte is stored after this one
    g.write32(p + 0, 0);
    g.write32(p + 4, 1);
    for (uint32_t off = 0x8; off <= 0x11; ++off)  // gate: equivalent 0x12: byte 0x12 is stored (1) afterwards
        g.write8(p + off, 0);  // gate: equivalent write16: the next byte is stored afterwards (by the loop, or +0x12 = 1)
    g.write8(p + 0x12, 1);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x13, 1);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x14, 1);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x15, 0);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x17, 0);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x18, 0);  // gate: equivalent write16: the next byte is stored after this one
    g.write8(p + 0x19, 0);
    g.ret_copy_of_arg(0);
}

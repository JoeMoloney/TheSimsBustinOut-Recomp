// Saves the current state as the pending one (the reverse of State_CommitFrom1C_00191480): words +0/+4/+8/+0x10 to +0x1C/+0x20/+0x24/+0x28, bytes +0x14..+0x17 to +0x2C..+0x2F, byte +0x18 to +0x30 (all read first), and sets the global 0x3F7E38 ($gp - 0x7CB8) to 1. Returns the byte from +0x18. 1 call site.
#include "theseus.h"

void State_SaveTo1C_00191410(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const uint32_t w0 = g.read32(p), w1 = g.read32(p + 4), w2 = g.read32(p + 8), w3 = g.read32(p + 0x10);
    const uint8_t b0 = g.read8(p + 0x14), b1 = g.read8(p + 0x15), b2 = g.read8(p + 0x16), b3 = g.read8(p + 0x17);  // gate: equivalent read16: kept as bytes
    const uint8_t flag = g.read8(p + 0x18);  // gate: equivalent read16: kept as a byte
    g.write32(p + 0x1C, w0);
    g.write8(p + 0x30, flag);
    g.write32(p + 0x20, w1);
    g.write32(p + 0x24, w2);
    g.write32(p + 0x28, w3);
    g.write8(p + 0x2C, b0);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x2D, b1);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x2E, b2);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x2F, b3);
    g.write32(0x3F7E38, 1);
    SET_GPR_S32(ctx, 12, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(flag);
}

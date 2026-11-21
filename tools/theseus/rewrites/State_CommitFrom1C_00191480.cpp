// Copies the pending state (+0x1C..+0x30) over the current one: words +0x1C/+0x20/+0x24/+0x28 to +0/+4/+8/+0x10, bytes +0x2C..+0x2F to +0x14..+0x17, byte +0x30 to +0x18 (all read first). Returns the byte from +0x30. 1 call site.
#include "theseus.h"

void State_CommitFrom1C_00191480(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const uint32_t w0 = g.read32(p + 0x1C), w1 = g.read32(p + 0x20), w2 = g.read32(p + 0x24), w3 = g.read32(p + 0x28);
    const uint8_t b0 = g.read8(p + 0x2C), b1 = g.read8(p + 0x2D), b2 = g.read8(p + 0x2E), b3 = g.read8(p + 0x2F);  // gate: equivalent read16: kept as bytes
    const uint8_t flag = g.read8(p + 0x30);  // gate: equivalent read16: kept as a byte
    g.write32(p, w0);
    g.write8(p + 0x18, flag);
    g.write32(p + 4, w1);
    g.write32(p + 8, w2);
    g.write32(p + 0x10, w3);
    g.write8(p + 0x14, b0);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x15, b1);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x16, b2);  // gate: equivalent write16: the next store overwrites the byte after
    g.write8(p + 0x17, b3);
    g.ret(flag);
}

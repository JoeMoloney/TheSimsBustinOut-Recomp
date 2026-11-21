// Copies six bytes from a0 (+0x406D, +0x406F, +0x4071, +0x406C, +0x4078, +0x4076) to a1
// (+0x12, +0x13, +0x14, +0x10, +0x11, +0x16), in the original's order. v0 = the byte read from
// a0+0x4078. 6 call sites.
#include "theseus.h"

void Bytes6_Gather4060_0011f788(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg(0), dst = g.arg(1);
    g.write8(dst + 0x12, g.read8(src + 0x406D));  // gate: equivalent read16,write16: a byte store; the next store overwrites the byte after
    g.write8(dst + 0x13, g.read8(src + 0x406F));  // gate: equivalent read16,write16: a byte store; the next store overwrites the byte after
    g.write8(dst + 0x14, g.read8(src + 0x4071));  // gate: equivalent read16: kept as a byte
    g.write8(dst + 0x10, g.read8(src + 0x406C));  // gate: equivalent read16,write16: the store to +0x11 below overwrites the byte after
    const uint8_t v0 = g.read8(src + 0x4078);  // gate: equivalent read16: kept as a byte
    g.write8(dst + 0x11, v0);
    g.write8(dst + 0x16, g.read8(src + 0x4076));  // gate: equivalent read16: kept as a byte
    g.ret(v0);
}

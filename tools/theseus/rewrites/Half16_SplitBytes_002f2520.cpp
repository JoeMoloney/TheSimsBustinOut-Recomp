// Splits the halfword at a0+0x16: *a1 = its high byte (signed), *a2 = its low byte (unsigned, read again after the first store). v0 = the high byte. 2 call sites.
#include "theseus.h"

void Half16_SplitBytes_002f2520(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t high = static_cast<int8_t>(g.read16(g.arg(0) + 0x16) >> 8);  // gate: equivalent read32: only bits 8-15 are kept
    g.write32(g.arg(1), static_cast<uint32_t>(high));
    g.write32(g.arg(2), g.read8(g.arg(0) + 0x16));
    g.ret(static_cast<uint32_t>(high));
}

// True if the signed 16-bit field +0x12 of the object that +0x18 of a0 points to is 2
// (17 call sites).
#include "theseus.h"

void Field18_Kind12Is2_0031eb60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t inner = g.read32(g.arg(0) + 0x18);
    g.ret(g.sread16(inner + 0x12) == 2 ? 1 : 0);  // gate: equivalent read16: sign only matters for negatives, never == 2
}

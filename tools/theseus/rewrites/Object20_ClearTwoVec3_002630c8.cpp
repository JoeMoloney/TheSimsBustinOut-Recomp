// Zeroes +0..+0xB, +0x10..+0x1B (two 12-byte vectors, through a zeroed stack temporary), +0x1C and +0xC. Returns 0 (v0 and v1 hold the copied zeros). 1 call site.
#include "theseus.h"

void Object20_ClearTwoVec3_002630c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write64(p, 0);
    g.write32(p + 8, 0);
    g.write64(p + 0x10, 0);
    g.write32(p + 0x18, 0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0xC, 0);
    SET_GPR_S32(ctx, 3, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret64(0);
}

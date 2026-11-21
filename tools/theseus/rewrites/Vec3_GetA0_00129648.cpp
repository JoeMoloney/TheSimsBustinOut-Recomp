// Copies the 12-byte vector at a0+0xA0 to a1 (8 + 4 bytes). v0 = the first 8 bytes (64 bits), v1 = the last word. 1 call site.
#include "theseus.h"

void Vec3_GetA0_00129648(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t lo = g.read64(g.arg(0) + 0xA0);
    const uint32_t hi = g.read32(g.arg(0) + 0xA8);
    g.write64(g.arg(1), lo);
    g.write32(g.arg(1) + 8, hi);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(hi));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret64(lo);
}

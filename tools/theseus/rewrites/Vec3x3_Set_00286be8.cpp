// Copies three 12-byte vectors into a0: *a1 to +0, *a2 to +0xC, *a3 to +0x18 (each as 8 + 4 bytes). v0 = the last word copied. 2 call sites.
#include "theseus.h"

void Vec3x3_Set_00286be8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0);
    uint64_t lo = g.read64(g.arg(1));
    uint32_t hi = g.read32(g.arg(1) + 8);
    g.write64(dst, lo);
    g.write32(dst + 8, hi);
    lo = g.read64(g.arg(2));
    hi = g.read32(g.arg(2) + 8);
    g.write64(dst + 0xC, lo);
    g.write32(dst + 0x14, hi);
    lo = g.read64(g.arg(3));
    hi = g.read32(g.arg(3) + 8);
    g.write64(dst + 0x18, lo);
    g.write32(dst + 0x20, hi);
    g.ret(hi);
}

// Copies three 12-byte vectors out of the object at a0: +0x390 to *a1, +0x3B0 to *a2, +0x39C
// to *a3 (each as 8 + 4 bytes, in the original's order). v0 = the word read from a0+0x3A4.
// 5 call sites. (The map joined another function's epilogue after the `jr $ra`.)
#include "theseus.h"

void Vec3x3_Get390_00104778(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    uint64_t lo = g.read64(obj + 0x390);
    uint32_t hi = g.read32(obj + 0x398);
    g.write64(g.arg(1), lo);
    g.write32(g.arg(1) + 8, hi);
    lo = g.read64(obj + 0x3B0);
    hi = g.read32(obj + 0x3B8);
    g.write64(g.arg(2), lo);
    g.write32(g.arg(2) + 8, hi);
    lo = g.read64(obj + 0x39C);
    hi = g.read32(obj + 0x3A4);
    g.write64(g.arg(3), lo);
    g.write32(g.arg(3) + 8, hi);
    g.ret(hi);
}

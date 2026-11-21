// Two "is this value near" tests on the object at a0; 1 if both pass, else 0:
//   first:  d = |a1 - [a0+0x20]|, w = [a0+0x2C4] ? 12 - d : 13; fails if [a0+0x2B8] < d and < w
//   second: d = |a2 - [a0+0x24]|, w = [a0+0x2C0] ? 12 - d : 12; fails if [a0+0x2BC] < d and < w
// (w is the distance the other way round a 12-step clock when the flag is set). Signed 32-bit
// arithmetic; a1/a2 are compared with the fields on all 64 bits to pick the sign. 3 call sites.
#include "theseus.h"

void TimeWindow_BothNear_0035bfc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t o = g.arg(0);
    const auto field = [&](uint32_t off) { return static_cast<int32_t>(g.read32(o + off)); };
    // |value - center| as the original computes it (subu), choosing the order by a 64-bit compare.
    const auto distance = [&](int arg, int32_t center) {
        return g.sarg64(arg) < center ?  // gate: equivalent <=: equal values give distance 0 either way
                static_cast<int32_t>(static_cast<uint32_t>(center) - g.arg(arg))
                                      : static_cast<int32_t>(g.arg(arg) - static_cast<uint32_t>(center));
    };
    const int32_t d1 = distance(1, field(0x20));
    const int32_t w1 = field(0x2C4) != 0 ? static_cast<int32_t>(12u - static_cast<uint32_t>(d1)) : 13;
    const int32_t tol1 = field(0x2B8);
    if (tol1 < d1 && tol1 < w1)
    {
        g.ret(0);
        return;
    }
    const int32_t d2 = distance(2, field(0x24));
    const int32_t w2 = field(0x2C0) != 0 ? static_cast<int32_t>(12u - static_cast<uint32_t>(d2)) : 12;
    const int32_t tol2 = field(0x2BC);
    g.ret(tol2 < d2 && tol2 < w2 ? 0 : 1);
}

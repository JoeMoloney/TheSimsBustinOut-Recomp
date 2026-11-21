// libgcc soft-float __unpack_d: splits the double at a0 into the fp_number_type at a1
// (class +0, sign +4, normal_exp +8, fraction +0x10 u64; see Fp_PackD_00229918).
//   exponent 0        -> ZERO (2); denormals are flushed, nothing else is stored
//   exponent 0x7FF    -> INF (4) if the fraction is 0, else QNAN (1, fraction bit 51 set) or
//                        SNAN (0), with the raw fraction
//   otherwise         -> NUMBER (3), exp - 0x3FF, fraction << 8 with the implicit 1 at bit 60
// Returns the class in v0, except 1 for both NaNs. 8 call sites.
#include "theseus.h"

void Fp_UnpackD_00229a48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t out = g.arg(1);
    const uint64_t bits = g.read64(g.arg(0));
    const uint64_t fraction = bits & 0x000FFFFFFFFFFFFFull;
    const uint32_t exp = static_cast<uint32_t>(bits >> 52) & 0x7FF;
    g.write32(out + 4, static_cast<uint32_t>(bits >> 63));
    if (exp == 0)
    {
        g.write32(out, 2);
        g.ret(2);
        return;
    }
    if (exp == 0x7FF)
    {
        if (fraction == 0)
        {
            g.write32(out, 4);
            g.ret(4);
            return;
        }
        g.write32(out, (fraction & (1ull << 51)) ? 1 : 0);
        g.write64(out + 0x10, fraction);
        g.ret(1);
        return;
    }
    g.write64(out + 0x10, (fraction << 8) | (1ull << 60));
    g.write32(out + 8, exp - 0x3FF);
    g.write32(out, 3);
    g.ret(3);
}

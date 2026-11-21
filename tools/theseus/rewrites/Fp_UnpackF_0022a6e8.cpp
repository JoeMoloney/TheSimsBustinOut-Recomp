// libgcc soft-float __unpack_f: splits the float at a0 into the fp_number_type at a1 (class +0, sign +4, normal_exp +8, fraction +0xC u32). Exponent 0 -> ZERO (2, denormals flushed); 0xFF -> INF (4) for fraction 0, else QNAN (1, fraction bit 20 set) or SNAN (0) with the raw fraction; otherwise NUMBER (3), exp - 0x7F, fraction << 7 with the implicit 1 at bit 30. Returns the class, except 1 for both NaNs. 2 call sites. (The map joined other functions' tails after the last `jr $ra`.)
#include "theseus.h"

void Fp_UnpackF_0022a6e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t out = g.arg(1), bits = g.read32(g.arg(0));
    const uint32_t fraction = bits & 0x7FFFFF, exp = (bits >> 23) & 0xFF;
    g.write32(out + 4, bits >> 31);
    if (exp == 0)
    {
        g.write32(out, 2);
        g.ret(2);
        return;
    }
    if (exp == 0xFF)
    {
        if (fraction == 0)
        {
            g.write32(out, 4);
            g.ret(4);
            return;
        }
        g.write32(out, (fraction & 0x100000) ? 1 : 0);
        g.write32(out + 0xC, fraction);
        g.ret(1);
        return;
    }
    g.write32(out + 0xC, (fraction << 7) | 0x40000000);
    g.write32(out + 8, exp - 0x7F);
    g.write32(out, 3);
    g.ret(3);
}

// libgcc soft-float _fpadd_parts(a = a0, b = a1, tmp = a2) on unpacked doubles (fp_number_type: class +0 (0 SNAN, 1 QNAN, 2 ZERO, 3 NUMBER, 4 INF), sign +4, normal_exp +8, fraction +0x10 u64). NaN a/b -> a/b; INF a -> a (or the NaN constant 0x44A380 for INF + INF of opposite signs); INF b -> b; ZERO b -> a, or for ZERO + ZERO a copy of a in tmp with sign a & b; ZERO a -> b. Otherwise the smaller exponent is raised to the larger by sticky right shifts (both fractions dropped to 0 / the smaller to 0 when they differ by 64 or more), the fractions are added or subtracted by sign into tmp, renormalised left below bit 60, and shifted right once (sticky) at bit 61 or above; tmp'\''s class becomes NUMBER. Returns the result pointer. 2 call sites.
#include "theseus.h"

void Fp_AddParts_00229ae8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1), tmp = g.arg(2);
    const uint32_t a_class = g.read32(a);
    if (a_class < 2)
    {
        g.ret(a);
        return;
    }
    const uint32_t b_class = g.read32(b);
    if (b_class < 2)
    {
        g.ret(b);
        return;
    }
    if (a_class == 4)
    {
        if (b_class == 4 && g.read32(a + 4) != g.read32(b + 4))
            g.ret(0x44A380);  // the NaN constant
        else
            g.ret(a);
        return;
    }
    if (b_class == 4)
    {
        g.ret(b);
        return;
    }
    if (b_class == 2)
    {
        if (a_class != 2)
        {
            g.ret(a);
            return;
        }
        g.write64(tmp, g.read64(a));
        g.write64(tmp + 8, g.read64(a + 8));
        g.write64(tmp + 0x10, g.read64(a + 0x10));
        g.write32(tmp + 4, g.read32(a + 4) & g.read32(b + 4));
        g.ret(tmp);
        return;
    }
    if (a_class == 2)
    {
        g.ret(b);
        return;
    }
    int32_t a_exp = static_cast<int32_t>(g.read32(a + 8));
    int32_t b_exp = static_cast<int32_t>(g.read32(b + 8));
    uint64_t a_frac = g.read64(a + 0x10), b_frac = g.read64(b + 0x10);
    int32_t diff = static_cast<int32_t>(static_cast<uint32_t>(a_exp) - static_cast<uint32_t>(b_exp));
    if (diff < 0)
        diff = static_cast<int32_t>(0u - static_cast<uint32_t>(diff));
    if (diff < 64)
    {
        if (b_exp < a_exp)
            for (; b_exp < a_exp; ++b_exp)
                b_frac = (b_frac >> 1) | (b_frac & 1);  // sticky shift
        else if (a_exp < b_exp)
        {
            for (int32_t n = b_exp - a_exp; n != 0; --n)
                a_frac = (a_frac >> 1) | (a_frac & 1);
            a_exp = b_exp;
        }
    }
    else if (b_exp < a_exp)
        b_frac = 0;
    else
    {
        a_exp = b_exp;
        a_frac = 0;
    }
    const uint32_t a_sign = g.read32(a + 4), b_sign = g.read32(b + 4);
    uint64_t frac;
    if (a_sign == b_sign)
    {
        frac = a_frac + b_frac;
        g.write32(tmp + 4, a_sign);
        g.write32(tmp + 8, static_cast<uint32_t>(a_exp));
        g.write64(tmp + 0x10, frac);
    }
    else
    {
        const int64_t t = static_cast<int64_t>(a_sign != 0 ? b_frac - a_frac : a_frac - b_frac);
        g.write32(tmp + 8, static_cast<uint32_t>(a_exp));
        if (t < 0)
        {
            g.write64(tmp + 0x10, 0 - static_cast<uint64_t>(t));
            g.write32(tmp + 4, 1);
        }
        else
        {
            g.write64(tmp + 0x10, static_cast<uint64_t>(t));
            g.write32(tmp + 4, 0);
        }
        frac = g.read64(tmp + 0x10);
        // renormalise: while 0 < frac < 1 << 60
        if (frac - 1 <= 0x0FFFFFFFFFFFFFFEull)
        {
            do
            {
                const int32_t e = static_cast<int32_t>(g.read32(tmp + 8));
                frac <<= 1;
                g.write64(tmp + 0x10, frac);
                g.write32(tmp + 8, static_cast<uint32_t>(e - 1));
            } while (frac - 1 <= 0x0FFFFFFFFFFFFFFEull);
        }
    }
    g.write32(tmp, 3);
    if (frac > 0x1FFFFFFFFFFFFFFFull)
    {
        const uint32_t e = g.read32(tmp + 8);
        g.write64(tmp + 0x10, (frac >> 1) | (frac & 1));
        g.write32(tmp + 8, e + 1);
    }
    g.ret(tmp);
}

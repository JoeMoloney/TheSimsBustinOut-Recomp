// libgcc soft-float pack_d (fp-bit.c, GCC 2.95): packs the unpacked double at a0 (class +0
// (0 SNAN, 1 QNAN, 2 ZERO, 3 NUMBER, 4 INFINITY), sign +4, normal_exp +8, fraction +0x10 (u64,
// with 8 guard bits below the 52 stored ones)) into an IEEE double, returned in all 64 bits of v0.
// Normal numbers round to nearest even on the guard bits; denormals are shifted down without
// rounding (this libgcc version); exponents from 1024 up give infinity. 6 call sites.
#include "theseus.h"

void Fp_PackD_00229918(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg(0);
    const uint32_t cls = g.read32(src);
    const uint64_t sign = g.read32(src + 4) & 1;  // gate: equivalent read16: only bit 0 is used
    uint64_t fraction = g.read64(src + 0x10);
    uint32_t exp = 0;
    if (cls < 2)  // NaN: quiet
    {
        exp = 0x7FF;
        fraction |= 1ull << 51;
    }
    else if (cls == 4)  // infinity
    {
        exp = 0x7FF;
        fraction = 0;
    }
    else if (cls == 2)  // zero
        fraction = 0;
    else if (fraction != 0)
    {
        const int32_t normal_exp = static_cast<int32_t>(g.read32(src + 8));
        if (normal_exp < -1022)  // denormal
        {
            const int32_t shift = static_cast<int32_t>(static_cast<uint32_t>(-1022) - static_cast<uint32_t>(normal_exp));
            fraction = shift < 57 ? (fraction >> (shift & 63)) >> 8 : 0;  // gate: equivalent <=,58: shifting by 57 and then 8 leaves 0 as well
        }
        else if (normal_exp >= 1024)  // overflow: infinity
        {
            exp = 0x7FF;
            fraction = 0;
        }
        else
        {
            exp = static_cast<uint32_t>(normal_exp) + 1023;
            if ((fraction & 0xFF) != 0x80)
                fraction += 0x7F;  // gate: equivalent 0x80: the low byte is not 0x80 here, so +0x80 changes nothing above bit 7
            else if (fraction & 0x100)  // gate: equivalent 0x101: here the low byte is 0x80, so bit 0 is 0  // exactly half: round to even
                fraction += 0x80;  // gate: equivalent 0x81: bit 0 is dropped by the final >> 8 (no carry: the low byte is 0x80)
            if (fraction > (~0ull >> 3))  // gate: equivalent >=: after rounding the fraction never equals 0x1FFF...F (its low byte would have been 0x80)  // carried into a new top bit
            {
                fraction >>= 1;
                exp += 1;
            }
            fraction >>= 8;
        }
    }
    g.ret64((fraction & 0x000FFFFFFFFFFFFFull) | (static_cast<uint64_t>(exp & 0x7FF) << 52) | (sign << 63));
}

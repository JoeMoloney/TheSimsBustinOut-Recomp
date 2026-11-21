// libgcc soft-float __fpcmp_parts_d (fp-bit.c): compares two unpacked doubles at a0 and a1
// (fp_number_type: class +0 (0 SNAN, 1 QNAN, 2 ZERO, 3 NUMBER, 4 INFINITY), sign +4,
// normal_exp +8 (signed), fraction +0x10 (u64)). Returns 1 if either is a NaN, else -1, 0 or 1
// as a < b, a == b, a > b. 1 call site.
#include "theseus.h"

void Fp_CmpPartsD_0022a1f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    // Results the original moves from v1 with movz/movn carry v1's upper half (the caller check sees v0).
    const __m128i v1_entry = GPR_VEC(ctx, 3);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    const auto ret_v1 = [&](uint32_t value) { g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(value))), v1_entry); };
    const uint32_t a_class = g.read32(a);
    if (a_class < 2 || g.read32(b) < 2)  // a NaN
    {
        g.ret(1);
        return;
    }
    const uint32_t b_class = g.read32(b);
    const uint32_t a_sign = g.read32(a + 4), b_sign = g.read32(b + 4);
    // a > b / a < b for the sign of a; the original takes the value from v1 when a's sign is 0
    const auto a_greater = [&]() { if (a_sign != 0) g.ret(0xFFFFFFFF); else ret_v1(1); };
    const auto a_less = [&]() { if (a_sign != 0) g.ret(1); else ret_v1(0xFFFFFFFF); };
    if (a_class == 4 && b_class == 4)  // both infinite
        g.ret(b_sign - a_sign);
    else if (a_class == 4)
        a_greater();
    else if (b_class == 4)
    {
        if (b_sign != 0) g.ret(1); else ret_v1(0xFFFFFFFF);
    }
    else if (a_class == 2 && b_class == 2)  // both zero
        g.ret(0);
    else if (a_class == 2)
    {
        if (b_sign != 0) ret_v1(1); else g.ret(0xFFFFFFFF);
    }
    else if (b_class == 2)
        a_greater();
    else if (a_sign != b_sign)
        a_greater();
    else
    {
        const int32_t a_exp = static_cast<int32_t>(g.read32(a + 8)), b_exp = static_cast<int32_t>(g.read32(b + 8));
        const uint64_t a_frac = g.read64(a + 0x10), b_frac = g.read64(b + 0x10);
        if (a_exp > b_exp)
            a_greater();
        else if (a_exp < b_exp)
            a_less();
        else if (a_frac > b_frac)
            a_greater();
        else if (a_frac < b_frac)
            a_less();
        else
            g.ret(0);
    }
}

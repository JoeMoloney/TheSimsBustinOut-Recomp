// Reference model of the EE's FPU (COP1) for the Unicorn runner: what a single-precision
// instruction produces on a real PS2, written independently of PS2Recomp's runtime
// (ps2_runtime_macros.h), so that diff-testing one against the other means something.
//
// PS2 floats are not IEEE 754: there are no infinities or NaNs (exponent 255 is just a bigger
// number, up to Fmax = 0x7FFFFFFF ~ 2^129), exponent 0 is always zero (no denormals), results
// that overflow clamp to +-Fmax, results that underflow become +-0, and the hardware does not
// round to nearest.
//
// Two arithmetic modes (UNICORN_FPU environment variable, read by unicorn_runner.cpp):
//   exact    (default) bit-exact hardware arithmetic: add/sub/mul/div/sqrt/rsqrt are a port of
//            PS2Float from PCSX2 pull request #12001 (github.com/PCSX2/pcsx2/pull/12001, commit
//            0392a64, "Implements Soft-Floats for the interpreters", GPL-3.0+; not merged into
//            PCSX2 as of Nov 2026). It models the hardware's adder (operands truncated to the
//            guard bits it keeps), Booth multiplier and radix-4 divider / square root, which its
//            authors checked against a real console.
//   nearest  the same PS2 range rules (no Inf/NaN/denormals, clamp to Fmax, flush to 0) but
//            IEEE round-to-nearest-even: the arithmetic PS2Recomp's runtime implements on the
//            host. With it, differences that remain are translation bugs, not rounding.
//            (Computed through double: for + - * / sqrt of floats, rounding twice through a
//            format with >= 2p+2 = 50 bits gives the correctly rounded float; double has 53.)
//
// Of FCR31, the condition bit C (bit 23) is modelled, and DIV.S's zero-divisor flags (D and SD,
// or I and SI for 0/0: the one flag rule PCSX2's two implementations agree on). The other
// cause/sticky flags (O, U, SO, SU, and the I/D of sqrt/rsqrt) are not: no instruction in this
// game reads FCR31 (there is no cfc1 in its FPU code; see work/cop1_census.py), so they are
// unobservable here, and PCSX2's implementations disagree on them.
#pragma once
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace ps2fpu
{
    enum class Mode { Exact, Nearest };

    constexpr uint32_t kSign = 0x80000000u, kFmax = 0x7FFFFFFFu, kC = 0x00800000u;

    inline uint32_t exponent(uint32_t x) { return (x >> 23) & 0xFF; }
    inline uint32_t mantissa(uint32_t x) { return x & 0x7FFFFF; }
    inline bool is_zero(uint32_t x) { return exponent(x) == 0; }  // exponent 0 = zero, whatever the mantissa
    inline uint32_t make(bool sign, uint32_t exp, uint32_t man) { return (sign ? kSign : 0u) | (exp << 23) | (man & 0x7FFFFF); }

    // ---------------------------------------------------------------- exact (port of PS2Float)
    namespace exact
    {
        inline uint32_t abs(uint32_t x) { return x & kFmax; }
        inline uint32_t negate(uint32_t x) { return x ^ kSign; }

        // The adder's exponent alignment: the smaller operand loses the bits below the guard
        // bit it keeps (PS2Float::Add's "MIN_FLOATING_POINT_VALUE << exp_diff" masks), then
        // DoAdd sums with 6 extra low bits and truncates.
        inline uint32_t do_add(uint32_t self, uint32_t other)
        {
            const int32_t diff = int32_t(exponent(self)) - int32_t(exponent(other));
            if (diff < 0)
                return do_add(other, self);
            if (diff >= 25)
                return self;
            constexpr int kRound = 6;
            const uint32_t s1 = uint32_t(int32_t(self) >> 31), s2 = uint32_t(int32_t(other) >> 31);
            const int32_t m1 = int32_t(((mantissa(self) | 0x800000) ^ s1) - s1);
            const int32_t m2 = int32_t(((mantissa(other) | 0x800000) ^ s2) - s2);
            const int32_t man = int32_t(uint32_t(m1) << kRound) + (int32_t(uint32_t(m2) << kRound) >> diff);
            int32_t absman = man < 0 ? -man : man;
            if (absman == 0)
                return 0;
            int32_t raw_exp = int32_t(exponent(self)) - kRound;
            static constexpr int32_t kNormalize[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8,
                                                     16, 16, 16, 16, 16, 16, 16, 16, 24, 24, 24, 24, 24, 24, 24};
            const int32_t amount = kNormalize[std::countl_zero(uint32_t(absman))];  // absman > 0
            raw_exp -= amount;
            absman <<= amount;
            const int32_t msb = 31 - std::countl_zero(uint32_t(absman) >> 23);
            raw_exp += msb;
            absman >>= msb;
            if (raw_exp > 255)
                return man < 0 ? 0xFFFFFFFFu : kFmax;
            if (raw_exp < 1)
                return man < 0 ? kSign : 0u;
            return (uint32_t(man) & kSign) | (uint32_t(raw_exp) << 23) | (uint32_t(absman) & 0x7FFFFF);
        }

        // An exponent-0 operand: the result is the other operand, or a zero, with the sign the
        // hardware picks (by comparing magnitudes as integers).
        inline uint32_t add_with_zero(uint32_t a, uint32_t b, bool b_negated_sign)
        {
            const bool sb = ((b & kSign) != 0) != b_negated_sign;
            const bool sign = abs(a) >= abs(b) ? (a & kSign) != 0 : sb;
            if (is_zero(a) && !is_zero(b))
                return make(sign, exponent(b), mantissa(b));
            if (!is_zero(a) && is_zero(b))
                return make(sign, exponent(a), mantissa(a));
            // both zero: -0 only when both are negative (after negating b for a subtraction)
            return ((a & kSign) && sb) ? kSign : 0u;
        }

        inline uint32_t align_and_add(uint32_t a, uint32_t b)  // b already negated for sub
        {
            int32_t diff = int32_t(exponent(a)) - int32_t(exponent(b));
            if (diff > 0 && diff < 25)
                b &= 0xFFFFFFFFu << (diff - 1);
            else if (diff < 0 && diff > -25)
                a &= 0xFFFFFFFFu << (-diff - 1);
            return do_add(a, b);
        }

        inline uint32_t add(uint32_t a, uint32_t b)
        {
            if (is_zero(a) || is_zero(b))
                return add_with_zero(a, b, false);
            return align_and_add(a, b);
        }

        inline uint32_t sub(uint32_t a, uint32_t b)
        {
            if (is_zero(a) || is_zero(b))
                return add_with_zero(a, b, true);
            // PS2Float::Sub masks b before negating it; the mask leaves the sign bit alone.
            int32_t diff = int32_t(exponent(a)) - int32_t(exponent(b));
            if (diff > 0 && diff < 25)
                b &= 0xFFFFFFFFu << (diff - 1);
            else if (diff < 0 && diff > -25)
                a &= 0xFFFFFFFFu << (-diff - 1);
            return do_add(a, negate(b));
        }

        // The multiplier's Booth-recoded partial products: the low product bits it drops make the
        // result differ from a truncated exact product in bit 15 of the low word sometimes.
        struct Booth { uint32_t data, negate; };
        inline Booth booth(uint32_t a, uint32_t b, uint32_t bit)
        {
            const uint32_t test = (bit ? b >> (bit * 2 - 1) : b << 1) & 7;
            a <<= (bit * 2);
            a += (test == 3 || test == 4) ? a : 0;
            const uint32_t neg = (test >= 4 && test <= 6) ? ~0u : 0;
            const uint32_t pos = 1u << (bit * 2);
            a ^= (neg & -pos);
            a &= (test >= 1 && test <= 6) ? ~0u : 0;
            return {a, neg & pos};
        }
        struct Sum3 { uint32_t lo, hi; };
        inline Sum3 add3(uint32_t a, uint32_t b, uint32_t c)
        {
            const uint32_t u = a ^ b;
            return {u ^ c, ((u & c) | (a & b)) << 1};
        }
        inline uint64_t mul_mantissa(uint32_t a, uint32_t b)
        {
            const uint64_t full = uint64_t(a) * uint64_t(b);
            Booth b0 = booth(a, b, 0), b1 = booth(a, b, 1), b2 = booth(a, b, 2), b3 = booth(a, b, 3);
            Booth b4 = booth(a, b, 4), b5 = booth(a, b, 5), b6 = booth(a, b, 6), b7 = booth(a, b, 7);
            Sum3 t0 = add3(b1.data, b2.data, b3.data);
            Sum3 t1 = add3(b4.data & ~0x7FFu, b5.data & ~0xFFFu, b6.data);
            t1.hi |= b6.negate | (b5.data & 0x800);
            b7.data |= (b5.data & 0x400) + b5.negate;
            Sum3 t2 = add3(b0.data, t0.lo, t0.hi);
            Sum3 t3 = add3(b7.data, t1.lo, t1.hi);
            Sum3 t4 = add3(t2.hi, t3.lo, t3.hi);
            Sum3 t5 = add3(t2.lo, t4.lo, t4.hi);
            t5.hi += b7.negate;
            t5.lo &= ~0x7FFFu;
            t5.hi &= ~0x7FFFu;
            const uint32_t ps2lo = t5.lo + t5.hi;
            return full - ((ps2lo ^ full) & 0x8000);
        }

        inline uint32_t mul(uint32_t a, uint32_t b, bool *overflow = nullptr)
        {
            const uint32_t sign = (a ^ b) & kSign;
            if (is_zero(a) || is_zero(b))
                return sign;
            int32_t e = int32_t(exponent(a)) + int32_t(exponent(b)) - 127;
            uint32_t m = uint32_t(mul_mantissa(mantissa(a) | 0x800000, mantissa(b) | 0x800000) >> 23);
            if (m > 0xFFFFFF)
                m >>= 1, ++e;
            if (e > 255)
            {
                if (overflow) *overflow = true;
                return sign | kFmax;
            }
            if (e < 1)
                return sign;
            return sign | (uint32_t(e) << 23) | (m & 0x7FFFFF);
        }

        // Radix-4 SRT divider / square root with carry-save remainder (DOI 10.1109/ARITH.1995.465363).
        struct Csa { uint32_t sum, carry; };
        inline Csa csa(uint32_t a, uint32_t b, uint32_t c)
        {
            const uint32_t u = a ^ b;
            return {u ^ c, ((a & b) | (u & c)) << 1};
        }
        inline int32_t quotient_select(Csa cur)
        {
            const uint32_t mask = (1u << 24) - 1;
            const int32_t test = int32_t(((cur.sum & ~mask) + cur.carry) | (cur.sum & mask));
            if (test >= (1 << 23))
                return 1;
            if (test < int32_t(~0u << 24))
                return -1;
            return 0;
        }

        inline uint32_t div(uint32_t a, uint32_t b)
        {
            const uint32_t sign = (a ^ b) & kSign;
            if (is_zero(b))
                return sign | kFmax;  // x/0 and 0/0 (flags D / I not modelled)
            if (is_zero(a))
                return sign;
            const uint32_t bm = (mantissa(b) | 0x800000) << 2;
            Csa cur = {(mantissa(a) | 0x800000) << 2, 0};
            uint32_t q = 0;
            int32_t qbit = 1;
            for (int i = 0; i < 25; ++i)
            {
                q = (q << 1) + uint32_t(qbit);
                const uint32_t add = qbit > 0 ? ~bm : qbit < 0 ? bm : 0;
                cur.carry += qbit > 0;
                const Csa next = csa(cur.sum, cur.carry, add);
                qbit = quotient_select(qbit ? next : cur);
                cur.sum = next.sum << 1;
                cur.carry = next.carry << 1;
            }
            int32_t e = int32_t(exponent(a)) - int32_t(exponent(b)) + 126;
            if (q >= (1u << 24))
                ++e, q >>= 1;
            if (e > 255)
                return sign | kFmax;
            if (e < 1)
                return sign;
            return (q & 0x7FFFFF) | (uint32_t(e) << 23) | sign;
        }

        inline uint32_t sqrt(uint32_t a)  // of |a|; never negative
        {
            if (is_zero(a))
                return 0;
            uint32_t m = (mantissa(a) | 0x800000) << 1;
            if (!(a & 0x800000))  // odd unbiased exponent
                m <<= 1;
            Csa cur = {m, 0};
            uint32_t q = 0;
            int32_t qbit = 1;
            for (int32_t i = 0; i < 25; ++i)
            {
                const uint32_t adjust = q + (uint32_t(qbit) << (24 - i));
                q += uint32_t(qbit) << (25 - i);
                const uint32_t add = qbit > 0 ? ~adjust : qbit < 0 ? adjust : 0;
                cur.carry += qbit > 0;
                const Csa next = csa(cur.sum, cur.carry, add);
                qbit = quotient_select(qbit ? next : cur);
                cur.sum = next.sum << 1;
                cur.carry = next.carry << 1;
            }
            const uint32_t e = (exponent(a) + 127) >> 1;
            return ((q >> 2) & 0x7FFFFF) | (e << 23);
        }

        // fs / sqrt(|ft|): the root is a full sqrt result, then a full division.
        inline uint32_t rsqrt(uint32_t s, uint32_t t) { return div(s, sqrt(t & kFmax)); }
    }

    // ---------------------------------------------------------------- nearest (IEEE rounding, PS2 range)
    namespace nearest
    {
        inline double value(uint32_t x)  // exponent 255 is an ordinary binade here
        {
            if (is_zero(x))
                return (x & kSign) ? -0.0 : 0.0;
            const double m = 1.0 + double(mantissa(x)) / 8388608.0;
            const double v = std::ldexp(m, int(exponent(x)) - 127);
            return (x & kSign) ? -v : v;
        }

        // Round a double to 24 significant bits, nearest-even; exponent 1..255 kept, else clamp/flush.
        inline uint32_t round(double d, bool *overflow = nullptr)
        {
            const uint32_t sign = std::signbit(d) ? kSign : 0u;
            const double mag = std::fabs(d);
            if (mag == 0.0)
                return sign;
            int e2;
            const double frac = std::frexp(mag, &e2);  // mag = frac * 2^e2, frac in [0.5, 1)
            const double scaled = std::ldexp(frac, 24);   // [2^23, 2^24)
            double r = std::nearbyint(scaled);           // default FE_TONEAREST = ties to even
            int e = e2 - 1 + 127;                         // biased exponent of 1.xxx
            if (r >= 16777216.0)
                r = 8388608.0, ++e;
            if (e > 255)
            {
                if (overflow) *overflow = true;
                return sign | kFmax;
            }
            if (e < 1)
                return sign;
            return sign | (uint32_t(e) << 23) | (uint32_t(r) & 0x7FFFFF);
        }

        inline uint32_t add(uint32_t a, uint32_t b) { return round(value(a) + value(b)); }
        inline uint32_t sub(uint32_t a, uint32_t b) { return round(value(a) - value(b)); }
        inline uint32_t mul(uint32_t a, uint32_t b, bool *overflow = nullptr)
        {
            if (is_zero(a) || is_zero(b))
                return (a ^ b) & kSign;
            return round(value(a) * value(b), overflow);
        }
        inline uint32_t div(uint32_t a, uint32_t b)
        {
            const uint32_t sign = (a ^ b) & kSign;
            if (is_zero(b))
                return sign | kFmax;
            if (is_zero(a))
                return sign;
            return round(value(a) / value(b));
        }
        inline uint32_t sqrt(uint32_t a) { return is_zero(a) ? 0u : round(std::sqrt(std::fabs(value(a)))); }
        inline uint32_t rsqrt(uint32_t s, uint32_t t) { return div(s, sqrt(t & kFmax)); }
    }

    // DIV.S's FCR31 flags for fs / ft.
    inline uint32_t div_flags(uint32_t fs, uint32_t ft)
    {
        if (!is_zero(ft))
            return 0;
        return is_zero(fs) ? 0x00020040u : 0x00010020u;  // I|SI for 0/0, D|SD for x/0
    }

    inline uint32_t add(Mode m, uint32_t a, uint32_t b) { return m == Mode::Exact ? exact::add(a, b) : nearest::add(a, b); }
    inline uint32_t sub(Mode m, uint32_t a, uint32_t b) { return m == Mode::Exact ? exact::sub(a, b) : nearest::sub(a, b); }
    inline uint32_t mul(Mode m, uint32_t a, uint32_t b) { return m == Mode::Exact ? exact::mul(a, b) : nearest::mul(a, b); }
    inline uint32_t div(Mode m, uint32_t a, uint32_t b) { return m == Mode::Exact ? exact::div(a, b) : nearest::div(a, b); }
    inline uint32_t sqrt(Mode m, uint32_t a) { return m == Mode::Exact ? exact::sqrt(a) : nearest::sqrt(a); }
    inline uint32_t rsqrt(Mode m, uint32_t s, uint32_t t) { return m == Mode::Exact ? exact::rsqrt(s, t) : nearest::rsqrt(s, t); }

    // MADD/MSUB family: the product is rounded first, then added to ACC (two operations, as in
    // PS2Float::MulAdd). Not used by this game (work/cop1_census.py), kept for completeness;
    // PS2Float's extra overflow rules for an overflowing product are applied.
    inline uint32_t madd(Mode m, uint32_t acc, uint32_t s, uint32_t t, bool subtract)
    {
        bool overflow = false;
        const uint32_t p = m == Mode::Exact ? exact::mul(s, t, &overflow) : nearest::mul(s, t, &overflow);
        if (overflow)  // PS2Float::DetermineMacException: +-Fmax by the product's sign (flipped for msub)
            return ((p & kSign) != 0) != subtract ? 0xFFFFFFFFu : kFmax;
        return subtract ? sub(m, acc, p) : add(m, acc, p);
    }

    // Comparisons: exponent-0 values are zero (both signs equal), everything else orders by
    // magnitude bits; no unordered case (PCSX2 PR #12001's fpuCompareFull).
    inline int32_t compare_key(uint32_t f)
    {
        if (is_zero(f))
            f = 0;
        if (f & kSign)
            f ^= 0x7FFFFFFFu;
        return int32_t(f);
    }

    // MAX.S / MIN.S compare the raw bits as integers (no zero/denormal special cases).
    inline uint32_t max(uint32_t a, uint32_t b)
    {
        const int32_t x = int32_t(a), y = int32_t(b);
        return uint32_t((x < 0 && y < 0) ? std::min(x, y) : std::max(x, y));
    }
    inline uint32_t min(uint32_t a, uint32_t b)
    {
        const int32_t x = int32_t(a), y = int32_t(b);
        return uint32_t((x < 0 && y < 0) ? std::max(x, y) : std::min(x, y));
    }

    // CVT.W.S: truncates; from exponent 158 (|x| >= 2^31) saturates by sign.
    inline uint32_t cvt_w_s(uint32_t f)
    {
        if ((f & 0x7F800000u) <= 0x4E800000u)
        {
            if (is_zero(f))
                return 0;
            const int e = int(exponent(f)) - 127;
            if (e < 0)
                return 0;
            const uint32_t m = mantissa(f) | 0x800000;
            const uint32_t mag = e >= 23 ? m << (e - 23) : m >> (23 - e);
            return (f & kSign) ? uint32_t(-int64_t(mag)) : mag;
        }
        return (f & kSign) ? 0x80000000u : 0x7FFFFFFFu;
    }

    // CVT.S.W: exact below 2^24; above, Exact mode truncates (the EE FPU only rounds toward
    // zero: an assumption, PCSX2 converts with its EE rounding mode, "chop" by default) and
    // Nearest mode rounds to nearest even.
    inline uint32_t cvt_s_w(Mode mode, uint32_t w)
    {
        const int32_t i = int32_t(w);
        if (i == 0)
            return 0;
        const bool neg = i < 0;
        uint64_t mag = neg ? uint64_t(-int64_t(i)) : uint64_t(i);
        int e = 63 - std::countl_zero(mag);  // position of the leading 1
        uint64_t m;
        if (e <= 23)
            m = mag << (23 - e);
        else
        {
            const int drop = e - 23;
            m = mag >> drop;
            if (mode == Mode::Nearest)
            {
                const uint64_t rest = mag & ((1ull << drop) - 1), half = 1ull << (drop - 1);
                if (rest > half || (rest == half && (m & 1)))
                    if (++m == (1ull << 24))
                        m >>= 1, ++e;
            }
        }
        return make(neg, uint32_t(e + 127), uint32_t(m));
    }
}

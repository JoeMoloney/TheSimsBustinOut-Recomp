// Binary search over 12-byte elements from a0 to a1 (end), keyed by the signed 16-bit value at
// +8, for key a2 (64-bit compares): the matching element or 0. The element count is
// (end - begin) / 12, computed as a multiply by 0xAAAAAAAB. With one element left the result is
// `movz v0, t0`: v0 gets the whole t0 register, whose upper 64 bits are t0's on entry.
// 6 call sites over 2 identical copies.
// theseus: copies 0x1d1d00
#include "theseus.h"

void SortedArray12_FindKey16_001d1c80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i entry_t0 = GPR_VEC(ctx, 8);  // only its upper half reaches v0 (not ABI-compared); gate: equivalent
    uint64_t t0 = g.arg64(0);  // left as the original leaves it (the caller check follows a read); scratch, not ABI-compared; gate: equivalent
    uint32_t lo = g.arg(0), hi = g.arg(1);
    const int64_t key = g.sarg64(2);
    for (;;)
    {
        const uint32_t product = (hi - lo) * 0xAAAAAAABu;
        const int32_t count = static_cast<int32_t>(product) >> 2;
        if (count <= 0)
        {
            SET_GPR_U64(ctx, 8, t0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(0);
            return;
        }
        if (count == 1)
        {
            SET_GPR_U64(ctx, 8, t0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            if (static_cast<int64_t>(g.sread16(lo + 8)) != key)
            {
                g.ret(0);
                return;
            }
            SET_GPR_VEC(ctx, 2, _mm_unpacklo_epi64(_mm_cvtsi64_si128(static_cast<int64_t>(static_cast<int32_t>(lo))),
                                                   _mm_unpackhi_epi64(entry_t0, entry_t0)));
            g.ret_void();
            return;
        }
        const int32_t half = count >> 1;  // signed / 2 in the original (count > 1 here, so the sign term is 0); gate: equivalent 2: any midpoint in range finds the same element
        const uint32_t mid = static_cast<uint32_t>(half) * 12 + lo;
        const int32_t diff = static_cast<int32_t>(static_cast<uint32_t>(key) - static_cast<uint32_t>(static_cast<int32_t>(g.sread16(mid + 8))));
        if (diff == 0)
        {
            SET_GPR_U64(ctx, 8, t0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(mid);
            return;
        }
        if (diff > 0)  // gate: equivalent >=: diff == 0 returned above
        {
            lo = mid + 12;
            t0 = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(lo)));
        }
        else
            hi = mid;
    }
}

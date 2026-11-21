// Binary search over 12-byte records [a0, a1) sorted by the signed 16-bit key at +0x6, for
// the key a2; returns the matching record or 0. The record count is computed as the compiler does
// it: prod = (a1 - lo) * 0xAAAAAAAB, n = prod >> 2 (exact division by 12); mid = lo +
// ((n + (prod >> 31)) >> 1) records; a2 - key (32-bit, signed) picks the half. With one record
// left the key is compared to all 64 bits of a2 and the record is returned as `movz v0, t0`
// leaves it: t0 was set from a0 with `daddu` (low 64 bits) and keeps its own upper half. 1 call site.
#include "theseus.h"

void Records12_BSearchKey16At6_00303998(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i t0_entry = GPR_VEC(ctx, 8);  // gate: equivalent 9: only v0's upper half (not part of the ABI compare)
    uint32_t lo = g.arg(0), hi = g.arg(1);
    for (;;)
    {
        const uint32_t prod = (hi - lo) * 0xAAAAAAABu;
        const int32_t n = static_cast<int32_t>(prod) >> 2;
        if (n <= 0)
        {
            g.ret(0);
            return;
        }
        if (n == 1)
        {
            if (static_cast<int64_t>(g.sread16(lo + 0x6)) != g.sarg64(2))
            {
                g.ret(0);
                return;
            }
            const uint64_t low = lo == g.arg(0) ? g.arg64(0) : static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(lo)));  // gate: equivalent 1: arg(1) only changes bits 32..63 of v0, not part of the ABI compare
            g.ret_with_upper(low, t0_entry);
            return;
        }
        const uint32_t mid = lo + static_cast<uint32_t>(n >> 1) * 12u;  // the original adds the sign bit of prod (0: n >= 2)
        const int32_t diff = static_cast<int32_t>(g.arg(2) - static_cast<uint32_t>(g.sread16(mid + 0x6)));
        if (diff == 0)
        {
            g.ret(mid);
            return;
        }
        if (diff > 0)  // gate: equivalent >=: diff == 0 returned above
            lo = mid + 12;
        else
            hi = mid;
    }
}

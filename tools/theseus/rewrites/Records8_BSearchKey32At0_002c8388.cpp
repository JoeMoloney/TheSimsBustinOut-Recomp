// Binary search over 8-byte records [a0, a1) sorted by the signed word key at +0, for the key
// a2 (as Records8_BSearchKey16_002c70b8 with a 32-bit key): mid = a0 + ((n + sign) >> 1)
// records; a2 - key (32-bit, signed) picks the half. With one record left the key is compared to
// all 64 bits of a2 and the record is returned as `movz v0, a0` leaves it. 1 call site.
#include "theseus.h"

void Records8_BSearchKey32At0_002c8388(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a0_entry = GPR_VEC(ctx, 4);  // gate: equivalent 5: only v0's upper half (not part of the ABI compare)
    uint32_t lo = g.arg(0), hi = g.arg(1);
    for (;;)
    {
        const uint32_t bytes = hi - lo;
        const int32_t n = static_cast<int32_t>(bytes) >> 3;
        if (n <= 0)
        {
            g.ret(0);
            return;
        }
        if (n == 1)
        {
            if (static_cast<int64_t>(static_cast<int32_t>(g.read32(lo + 0))) != g.sarg64(2))
            {
                g.ret(0);
                return;
            }
            const uint64_t low = lo == g.arg(0) ? g.arg64(0) : static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(lo)));  // gate: equivalent 1: arg(1) only changes bits 32..63 of v0, not part of the ABI compare
            g.ret_with_upper(low, a0_entry);
            return;
        }
        const uint32_t mid = lo + static_cast<uint32_t>(n >> 1) * 8;  // the original adds the sign bit of bytes (0: n >= 2)
        const int32_t diff = static_cast<int32_t>(g.arg(2) - g.read32(mid + 0));
        if (diff == 0)
        {
            g.ret(mid);
            return;
        }
        if (diff > 0)  // gate: equivalent >=: diff == 0 returned above
            lo = mid + 8;
        else
            hi = mid;
    }
}

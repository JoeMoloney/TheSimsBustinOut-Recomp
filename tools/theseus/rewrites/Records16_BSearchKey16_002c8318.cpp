// Binary search over 16-byte records [a0, a1) sorted by the s16 key at +4, for the key a2;
// returns the matching record or 0. Written as the original loops: mid = lo + (n >> 1)
// records (n = (a1 - a0) / 16); a2 - key decides the half (32-bit, signed). With one record left
// the key is compared to all 64 bits of a2 and the record is returned as `movz v0, a0` leaves
// it: a0 unchanged (all 128 bits) if it never moved, else sign-extended with a0's upper half.
#include "theseus.h"

void Records16_BSearchKey16_002c8318(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a0_entry = GPR_VEC(ctx, 4);  // gate: equivalent 5: only v0's upper half (not part of the ABI compare)
    uint32_t lo = g.arg(0), hi = g.arg(1);
    for (;;)
    {
        const uint32_t bytes = hi - lo;
        const int32_t n = static_cast<int32_t>(bytes) >> 4;
        if (n <= 0)
        {
            g.ret(0);
            return;
        }
        if (n == 1)
        {
            if (static_cast<int64_t>(g.sread16(lo + 4)) != g.sarg64(2))
            {
                g.ret(0);
                return;
            }
            const uint64_t low = lo == g.arg(0) ? g.arg64(0) : static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(lo)));  // gate: equivalent 1: arg(1) only changes bits 32..63 of v0, not part of the ABI compare
            g.ret_with_upper(low, a0_entry);
            return;
        }
        const uint32_t mid = lo + static_cast<uint32_t>(n >> 1) * 16;  // the original adds the sign bit of bytes (0: n >= 2)
        const int32_t diff = static_cast<int32_t>(g.arg(2) - static_cast<uint32_t>(g.sread16(mid + 4)));
        if (diff == 0)
        {
            g.ret(mid);
            return;
        }
        if (diff > 0)  // gate: equivalent >=: diff == 0 returned above
            lo = mid + 16;
        else
            hi = mid;
    }
}

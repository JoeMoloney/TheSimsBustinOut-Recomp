// Binary search for a1 in the array of 8-byte entries at +0 (count at +4) of a0, keyed by the
// entry's first word (unsigned, all 64 bits): the index of a match, else where a1 would be
// inserted (3 call sites).
#include "theseus.h"

void SortedArray8_LowerBound_00277a68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint64_t key = g.arg64(1);
    int32_t lo = 0, hi = static_cast<int32_t>(g.read32(a + 4));
    const uint32_t array = g.read32(a);  // the original reads it only if hi > 0; a plain read has no effect
    while (lo < hi)
    {
        const int32_t mid = (lo + hi) >> 1;
        const uint64_t here = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(array + 8 * mid))));
        if (here < key)
            lo = mid + 1;
        else if (key < here)
            hi = mid;
        else
        {
            g.ret(static_cast<uint32_t>(mid));
            return;
        }
    }
    g.ret(static_cast<uint32_t>(lo));
}

// Binary search for a0 in the sorted (unsigned) word array a1 of a2 elements: the lower bound
// index if the element there equals a0 (all 64 bits), else -1 (also for a2 == 0). 1 call site.
#include "theseus.h"

void U32Array_BinaryFind_002767a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a3_entry = GPR_VEC(ctx, 7);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    const uint64_t key = g.arg64(0);
    const uint32_t array = g.arg(1);
    if (g.arg64(2) == 0)
    {
        g.ret(0xFFFFFFFF);
        return;
    }
    const auto element = [&](int32_t i) {
        return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(array + (static_cast<uint32_t>(i) << 2)))));
    };
    int32_t lo = 0, hi = static_cast<int32_t>(g.arg(2) - 1);
    while (lo < hi)
    {
        const int32_t mid = static_cast<int32_t>(static_cast<uint32_t>(lo) + static_cast<uint32_t>(hi)) >> 1;
        if (element(mid) < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    // found: `movz v0, a3` copies all of a3 (the index register keeps its upper half); the caller check sees v0
    if (element(lo) == key)
        g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(lo)), a3_entry);
    else
        g.ret(0xFFFFFFFF);
}

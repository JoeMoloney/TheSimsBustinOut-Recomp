// Walks the list starting at +0x30 of a0 (next at +0x1C) summing word +8 over runs of nodes
// that have word +0xC == 0 or bit 0 of word +0 set; any other node ends a run. Returns the
// largest run sum (signed). The final `movz v0, a0` copies the whole a0 register, so v0's upper
// 64 bits come from a0 (or from a1, once `movn a0, a1` has fired), as in the original. 3 call sites.
#include "theseus.h"

void List1C_MaxRunSum_002aa6d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i entry_a0 = GPR_VEC(ctx, 4), entry_a1 = GPR_VEC(ctx, 5);  // only v0's upper half (not ABI-compared); gate: equivalent
    int32_t best = 0, run = 0;
    bool best_from_run = false;  // `movn a0, a1` executed: a0's upper half is now a1's
    for (uint32_t node = g.read32(g.arg(0) + 0x30); node != 0; node = g.read32(node + 0x1C))
    {
        const uint32_t weight = g.read32(node + 0xC);
        const uint32_t flags = g.read32(node);  // gate: equivalent read16: only bit 0 is used
        if (weight == 0 || (flags & 1))
        {
            run += static_cast<int32_t>(g.read32(node + 8));
            continue;
        }
        if (best < run)  // gate: equivalent <=: on a tie only v0's upper half (not ABI-compared) changes
        {
            best = run;
            best_from_run = true;
        }
        run = 0;
    }
    if (best < run)  // gate: equivalent <=: on a tie only v0's upper half (not ABI-compared) changes
    {
        g.ret(static_cast<uint32_t>(run));
        return;
    }
    const __m128i upper_from = best_from_run ? entry_a1 : entry_a0;
    SET_GPR_VEC(ctx, 2, _mm_unpacklo_epi64(_mm_cvtsi64_si128(static_cast<int64_t>(best)), _mm_unpackhi_epi64(upper_from, upper_from)));
    g.ret_void();
}

// Entries of 20 bytes at [a0+0x20]: if entry a1 has a non-zero word at +0xC, returns it. Otherwise
// searches back from entry a1 - 1 to entry 0 for one with a non-zero +0xC value f and returns
// f + next.word0 * max(next.word4, 1), next being the entry after it; 0 if there is none (or if
// a1 <= 0, a1 on all 64 bits). 1 call site.
#include "theseus.h"

void Entries20_PrevEnd_002805d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a2_entry = GPR_VEC(ctx, 6);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    const uint32_t base = g.read32(g.arg(0) + 0x20);
    const auto entry = [&](uint32_t i) { return base + i * 20; };
    const uint32_t own = g.read32(entry(g.arg(1)) + 0xC);
    if (own != 0 || g.sarg64(1) <= 0)
    {
        g.ret(own);
        return;
    }
    for (int32_t j = static_cast<int32_t>(g.arg(1) - 1);; --j)
    {
        const uint32_t f = g.read32(entry(static_cast<uint32_t>(j)) + 0xC);
        if (f != 0)
        {
            const uint32_t next = entry(static_cast<uint32_t>(j) + 1);
            const uint32_t m = g.read32(next + 4);
            const uint32_t result = g.read32(next) * (m != 0 ? m : 1) + f;
            if (m == 0)  // `movz v0, a2` (a2 = 1) copied all of a2, so v0 keeps a2's upper half; only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
                g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(result))), a2_entry);
            else
                g.ret(result);
            return;
        }
        if (j <= 0)
            break;
    }
    g.ret(0);
}

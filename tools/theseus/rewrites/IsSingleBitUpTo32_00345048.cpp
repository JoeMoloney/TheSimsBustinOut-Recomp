// 1 if a0 has exactly one bit set and is at most 32 (signed), else 0: counts bits by clearing
// the lowest set bit (x &= x - 1, on all 64 bits with a 32-bit subtract, as `addiu`/`and`).
// 9 call sites.
#include "theseus.h"

void IsSingleBitUpTo32_00345048(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t x = g.sarg64(0);
    uint64_t v = static_cast<uint64_t>(x);
    uint32_t bits = 0;
    while (v != 0)
    {
        v &= static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(static_cast<uint32_t>(v) - 1)));
        ++bits;
    }
    g.ret(bits == 1 && !(32 < x) ? 1 : 0);  // gate: equivalent 33: no single-bit value lies in 33..33
}

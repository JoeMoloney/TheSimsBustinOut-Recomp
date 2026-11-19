// Whether a1 lies in [base, base + size) with base = word +0 and size = word +0x20 of a0
// (unsigned compares on all 64 bits, the sum sign-extended from 32 bits). 4 call sites.
#include "theseus.h"

void Range_Contains_002b05e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint32_t base = g.read32(a);
    const uint64_t x = g.arg64(1);
    if (x < static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(base))))
    {
        g.ret(0);
        return;
    }
    const uint32_t end = base + g.read32(a + 0x20);
    g.ret(x < static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(end))) ? 1 : 0);
}

// Advances two counters that cycle 0..11: *a2 becomes a4 (t0) when it equals the 9th argument (stack +0), else wraps 11 -> 0 or increments. When *a2 then equals a6 (t2), *a1 is advanced the same way (a7 (t3) as its special value, a3 as its replacement) and the result is whether *a1 differs from a5 (t1); otherwise returns 1. Values are compared with whole registers. 2 call sites.
#include "theseus.h"

void Cycle12_AdvancePair_0035c290(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t lo = g.arg(2), hi = g.arg(1);
    const int32_t special = static_cast<int32_t>(g.read32(GPR_U32(ctx, 29)));
    const int32_t a = static_cast<int32_t>(g.read32(lo));
    g.write32(lo, a == special ? g.arg(4) : a == 11 ? 0 : static_cast<uint32_t>(a + 1));
    if (static_cast<int32_t>(g.read32(lo)) != g.sarg64(6))
    {
        g.ret(1);
        return;
    }
    const int32_t b = static_cast<int32_t>(g.read32(hi));
    g.write32(hi, b == g.sarg64(7) ? g.arg(3) : b == 11 ? 0 : static_cast<uint32_t>(b + 1));
    const uint64_t now = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(hi))));
    g.ret((now ^ g.arg64(5)) != 0 ? 1 : 0);
}

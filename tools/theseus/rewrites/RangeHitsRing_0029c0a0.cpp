// Ring region [S, E) with S = [0x3F8270], E = [0x3F8274] (gp-0x7880 / gp-0x787C), a1 = start,
// a2 = length; all compares unsigned, a1 on all 64 bits, start + length on 32:
//   S == E: 0
//   E < S (wrapped): a1 < S and start + length >= E
//   E > S: a1 < S, or start + length >= E
// 2 call sites.
#include "theseus.h"

void RangeHitsRing_0029c0a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const auto sext = [](uint32_t v) { return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(v))); };
    const uint64_t s = sext(g.read32(0x003F8270));
    const uint64_t e = sext(g.read32(0x003F8274));
    const uint64_t start = g.arg64(1);
    const uint64_t end = sext(g.arg(1) + g.arg(2));
    if (s == e)
        g.ret(0);
    else if (e < s)  // gate: equivalent <=: s == e returned above
        g.ret(start < s && !(end < e) ? 1 : 0);
    else
        g.ret(start < s || !(end < e) ? 1 : 0);
}

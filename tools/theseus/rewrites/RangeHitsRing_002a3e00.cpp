// Ring region [S, E) with S = [gp-0x7844], E = [gp-0x7840], a1 = start, a2 = length (as
// RangeHitsRing_0029c0a0 with other globals); all compares unsigned, a1 on all 64 bits, start +
// length on 32:
//   S == E: 0;  E < S (wrapped): a1 < S and start + length >= E;  E > S: a1 < S, or start + length >= E
// 1 call site.
#include "theseus.h"

void RangeHitsRing_002a3e00(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const auto sext = [](uint32_t v) { return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(v))); };
    const uint64_t s = sext(g.read32(g.gp() - 0x7844));
    const uint64_t e = sext(g.read32(g.gp() - 0x7840));
    const uint64_t start = g.arg64(1);
    const uint64_t end = sext(g.arg(1) + g.arg(2));
    if (s == e)
        g.ret(0);
    else if (e < s)  // gate: equivalent <=: s == e returned above
        g.ret(start < s && !(end < e) ? 1 : 0);
    else
        g.ret(start < s || !(end < e) ? 1 : 0);
}

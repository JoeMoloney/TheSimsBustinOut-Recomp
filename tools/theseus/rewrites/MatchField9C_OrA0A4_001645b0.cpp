// With a1 != 0: whether a2 equals word +0x9C of a0; otherwise whether a2 equals word +0xA0
// or word +0xA4 (64-bit compares). 3 call sites.
#include "theseus.h"

void MatchField9C_OrA0A4_001645b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const int64_t want = g.sarg64(2);
    auto field = [&](uint32_t off) { return static_cast<int64_t>(static_cast<int32_t>(g.read32(a + off))); };
    if (g.arg64(1) != 0)
        g.ret(want == field(0x9C) ? 1 : 0);
    else
        g.ret(want == field(0xA0) || want == field(0xA4) ? 1 : 0);
}

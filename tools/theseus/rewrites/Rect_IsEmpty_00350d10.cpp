// 0 if word +0 < word +8 and word +4 < word +0xC of a0 (signed), else 1: an empty-rectangle
// test (4 call sites).
#include "theseus.h"

void Rect_IsEmpty_00350d10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    auto at = [&](uint32_t off) { return static_cast<int32_t>(g.read32(a + off)); };
    g.ret(at(0) < at(8) && at(4) < at(0xC) ? 0 : 1);
}

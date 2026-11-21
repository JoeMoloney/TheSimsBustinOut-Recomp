// a1 is a vector (begin +0, end +4) of 20-byte records starting with a rectangle {x0, y0, x1, y1}
// (signed). Returns the index of the first record that overlaps the rectangle at a0 (strictly:
// a0.y0 < r.y1, r.y0 < a0.y1, a0.x0 < r.x1, r.x0 < a0.x1), or -1. The index is computed as the
// compiler does it: (p - begin) * 0xCCCCCCCD >> 2 (exact division by 20). 2 call sites.
#include "theseus.h"

void Rects20_FirstOverlap_003704e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rect = g.arg(0), vec = g.arg(1);
    const uint32_t end = g.read32(vec + 4);
    const auto field = [&](uint32_t addr) { return static_cast<int32_t>(g.read32(addr)); };
    for (uint32_t r = g.read32(vec); r != end; r += 20)
    {
        if (field(rect + 4) < field(r + 0xC) && field(r + 4) < field(rect + 0xC) &&
            field(rect + 0) < field(r + 8) && field(r + 0) < field(rect + 8))
        {
            g.ret(static_cast<int32_t>((r - g.read32(vec)) * 0xCCCCCCCDu) >> 2);
            return;
        }
    }
    g.ret(0xFFFFFFFF);
}

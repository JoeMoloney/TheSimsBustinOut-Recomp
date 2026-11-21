// Searches two vectors of 20-byte records {x0, y0, x1, y1, priority} for records overlapping the
// rectangle at a1 (strictly, as Rects20_FirstOverlap_003704e0): first the vector at [a0+4]
// (begin +0, end +4), then, if [a0+8] is not null, the vector it points to. Returns the last
// overlapping record, or 0; an overlapping record whose priority (+0x10) is 0x7FFFFFFF is
// returned at once. 3 call sites.
#include "theseus.h"

void Rects20_LastOverlapTwoLists_00370840(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rect = g.arg(1);
    const auto field = [&](uint32_t addr) { return static_cast<int32_t>(g.read32(addr)); };
    uint32_t found = 0;
    const uint32_t first_vec = g.read32(g.arg(0) + 4);
    // Returns true when a record with the top priority ends the search.
    const auto search = [&](uint32_t vec) {
        const uint32_t end = g.read32(vec + 4);
        // t1 as the original leaves it (callers' callees may read it): the first vector's end, then
        // the rectangle's y0 if the second vector is searched and not empty
        if (g.read32(vec) != end) SET_GPR_S32(ctx, 9, vec == first_vec ? end : g.read32(rect + 4));  // gate: equivalent
        for (uint32_t r = g.read32(vec); r != end; r += 20)
        {
            if (field(rect + 4) < field(r + 0xC) && field(r + 4) < field(rect + 0xC) &&
                field(rect + 0) < field(r + 8) && field(r + 0) < field(rect + 8))
            {
                found = r;
                if (field(r + 0x10) == 0x7FFFFFFF)
                    return true;
            }
        }
        return false;
    };
    if (!search(first_vec))
        if (const uint32_t second = g.read32(g.arg(0) + 8); second != 0)
            search(second);
    g.ret(found);
}

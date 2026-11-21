// 1 if a1 is null (all 64 bits) or the rectangle {x0, y0, x1, y1} at a0 lies inside the one at a1
// (signed compares), else 0. 1 call site.
#include "theseus.h"

void Rect_InsideRectOrNull_003713d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) == 0)
    {
        g.ret(1);
        return;
    }
    const auto field = [&](int arg, uint32_t off) { return static_cast<int32_t>(g.read32(g.arg(arg) + off)); };
    const bool inside = !(field(0, 0) < field(1, 0)) && !(field(1, 8) < field(0, 8)) &&
                        !(field(0, 4) < field(1, 4)) && !(field(1, 0xC) < field(0, 0xC));
    g.ret(inside ? 1 : 0);
}

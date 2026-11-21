// 1 if the rectangle {x0, y0, x1, y1} at a0 lies inside the one at a1 (a1.x0 <= a0.x0,
// a1.y0 <= a0.y0, a0.x1 <= a1.x1, a0.y1 <= a1.y1; signed), else 0. 1 call site.
#include "theseus.h"

void Rect_InsideRect_00350d48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const auto field = [&](int arg, uint32_t off) { return static_cast<int32_t>(g.read32(g.arg(arg) + off)); };
    const bool inside = !(field(1, 0xC) < field(0, 0xC)) && !(field(0, 4) < field(1, 4)) &&
                        !(field(0, 0) < field(1, 0)) && !(field(1, 8) < field(0, 8));
    g.ret(inside ? 1 : 0);
}

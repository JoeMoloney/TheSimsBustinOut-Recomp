// Red-black tree rotate_left with a sentinel node 0x3A8AE8 (NIL) instead of null: tree a0 (root at +8, 0 above the root), node x = a1 (left +0, right +4, parent +8). y = x'\''s child at +4 takes x'\''s place; y'\''s child at +0 (unless NIL) becomes x'\''s child at +4. y'\''s +0 = x is stored even when y is NIL. Returns 0x3A8AE8. 2 call sites.
#include "theseus.h"

void RbTreeNil3A8AE8_Rotate_left_002bc428(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t nil = 0x3A8AE8, tree = g.arg(0), x = g.arg(1);
    const int64_t x64 = g.sarg64(1);  // compared as a whole register
    const auto is_x = [&](uint32_t p) { return static_cast<int64_t>(static_cast<int32_t>(p)) == x64; };
    const uint32_t y = g.read32(x + 4);
    g.write32(x + 4, g.read32(y + 0));
    const uint32_t inner = g.read32(y + 0);
    if (inner != nil)
        g.write32(inner + 8, x);
    if (y != nil)
        g.write32(y + 8, g.read32(x + 8));
    const uint32_t parent = g.read32(x + 8);
    if (parent == 0)
        g.write32(tree + 8, y);
    else if (is_x(g.read32(parent + 0)))
        g.write32(parent + 0, y);
    else
        g.write32(parent + 4, y);
    g.write32(y + 0, x);
    if (x64 != static_cast<int32_t>(nil))
        g.write32(x + 8, y);
    g.ret(nil);
}

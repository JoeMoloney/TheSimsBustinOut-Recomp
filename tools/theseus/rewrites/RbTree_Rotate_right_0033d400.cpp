// _Rb_tree_rotate_right(x = a0, root = *a1); node: parent +4, left +8, right +0xC. y = x's child at +8 takes x's place; y's child at +0xC becomes x's child at +8. v0 is what the original leaves: x's old parent when x was the root, else that parent's child pointer at +0xC. 2 call sites.
#include "theseus.h"

void RbTree_Rotate_right_0033d400(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t x = g.arg(0), root = g.arg(1);
    const int64_t x64 = g.sarg64(0);  // compared as a whole register
    const auto is_x = [&](uint32_t p) { return static_cast<int64_t>(static_cast<int32_t>(p)) == x64; };
    const uint32_t y = g.read32(x + 8);
    g.write32(x + 8, g.read32(y + 0xC));
    const uint32_t inner = g.read32(y + 0xC);
    if (inner != 0)
        g.write32(inner + 4, x);
    const uint32_t parent = g.read32(x + 4);
    g.write32(y + 4, parent);
    uint32_t v0 = parent;
    if (is_x(g.read32(root)))
        g.write32(root, y);
    else
    {
        const uint32_t p = g.read32(x + 4);
        v0 = g.read32(p + 0xC);
        g.write32(p + (is_x(v0) ? 0xC : 8), y);
    }
    g.write32(y + 0xC, x);
    g.write32(x + 4, y);
    g.ret(v0);
}

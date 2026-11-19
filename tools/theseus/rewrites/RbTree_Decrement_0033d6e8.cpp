// std::map/set iterator -- (GCC 2.95 _Rb_tree_base_iterator::_M_decrement). From the header
// (end(): red, and its parent's parent is itself) it returns the rightmost node; otherwise the
// previous node in key order. 9 call sites.
#include "theseus.h"

void RbTree_Decrement_0033d6e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.arg(0);
    if (g.read32(node) == 0 && g.read32(g.read32(node + 4) + 4) == node)  // gate: equivalent read16: colors are 0 or 1
    {
        g.ret(g.read32(node + 0xC));
        return;
    }
    const uint32_t left = g.read32(node + 8);
    if (left != 0)
    {
        node = left;  // rightmost node of the left subtree
        for (uint32_t right; (right = g.read32(node + 0xC)) != 0;)
            node = right;
        g.ret(node);
        return;
    }
    uint32_t parent = g.read32(node + 4);  // climb while we are a left child
    while (node == g.read32(parent + 8))
    {
        node = parent;
        parent = g.read32(node + 4);
    }
    g.ret(parent);
}

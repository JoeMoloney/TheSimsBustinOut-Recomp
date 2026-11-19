// std::map/set iterator ++ (GCC 2.95 _Rb_tree_base_iterator::_M_increment). Node: color +0,
// parent +4, left +8, right +0xC. Returns the next node in key order; from the rightmost node
// it returns the header (end()). 31 call sites.
#include "theseus.h"

void RbTree_Increment_0012b108(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.arg(0);
    const uint32_t right = g.read32(node + 0xC);
    if (right != 0)
    {
        node = right;  // leftmost node of the right subtree
        for (uint32_t left; (left = g.read32(node + 8)) != 0;)
            node = left;
        g.ret(node);
        return;
    }
    uint32_t parent = g.read32(node + 4);  // climb while we are a right child
    while (node == g.read32(parent + 0xC))
    {
        node = parent;
        parent = g.read32(node + 4);
    }
    // At the root with no right subtree the climb ends on the header, whose parent is the root
    // again: then the header itself is the answer.
    if (g.read32(node + 0xC) != parent)
        node = parent;
    g.ret(node);
}

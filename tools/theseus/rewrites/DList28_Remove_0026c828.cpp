// Unlinks node a1 from the doubly linked list at a0 (head +0x28, tail +0x2C, cursor +0x30;
// node next +0x178, prev +0x17C). A cursor on the node moves to its next. Returns the new
// tail when a1 was the tail, else the next node. 4 call sites. Pairs with
// DList28_AppendUnique_0026c7d0.
#include "theseus.h"

void DList28_Remove_0026c828(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t list = g.arg(0), node = g.arg(1);
    const int64_t node64 = g.sarg64(1);  // compared as a whole register
    const auto is_node = [&](uint32_t p) { return static_cast<int64_t>(static_cast<int32_t>(p)) == node64; };
    if (is_node(g.read32(list + 0x30)))
        g.write32(list + 0x30, g.read32(node + 0x178));
    if (is_node(g.read32(list + 0x28)))
        g.write32(list + 0x28, g.read32(node + 0x178));
    else
        g.write32(g.read32(node + 0x17C) + 0x178, g.read32(node + 0x178));
    if (is_node(g.read32(list + 0x2C)))
    {
        const uint32_t prev = g.read32(node + 0x17C);
        g.write32(list + 0x2C, prev);
        g.ret(prev);
        return;
    }
    const uint32_t prev = g.read32(node + 0x17C), next = g.read32(node + 0x178);
    g.write32(next + 0x17C, prev);
    g.ret(next);
}

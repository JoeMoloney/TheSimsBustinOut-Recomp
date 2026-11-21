// Removes a1 from the singly linked list whose head is at a0+0x404 (link +0x38). The node must be in the list (no end test, as in the original). Returns a1'\''s next. 1 call site. Pairs with List404_Push_0035e880.
#include "theseus.h"

void List404_Remove_0035e890(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t node64 = g.sarg64(1);  // compared as a whole register
    const auto is_node = [&](uint32_t p) { return static_cast<int64_t>(static_cast<int32_t>(p)) == node64; };
    uint32_t prev = 0;
    const uint32_t head = g.read32(g.arg(0) + 0x404);
    if (!is_node(head))
    {
        prev = head;
        uint32_t next;
        while (!is_node(next = g.read32(prev + 0x38)))
            prev = next;
    }
    const uint32_t after = g.read32(g.arg(1) + 0x38);
    if (prev != 0)
        g.write32(prev + 0x38, after);
    else
        g.write32(g.arg(0) + 0x404, after);
    g.ret(after);
}

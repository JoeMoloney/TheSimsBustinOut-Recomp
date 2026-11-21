// Removes a0 from the global list 0x3F8538 ($gp - 0x75B8; link at +4). Returns a0'\''s next if found, else 0. 1 call site. Pairs with List3F8538_Push_00339fd0.
#include "theseus.h"

void List3F8538_Remove_00339fe8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t target = g.sarg64(0);  // compared as a whole register
    uint32_t link = 0x3F8538;
    uint32_t node = g.read32(link);
    if (node == 0)
    {
        g.ret(0);
        return;
    }
    for (;;)
    {
        node = g.read32(link);
        const uint32_t next = g.read32(node + 4);
        if (static_cast<int64_t>(static_cast<int32_t>(node)) == target)
        {
            g.write32(link, next);
            g.ret(next);
            return;
        }
        if (next == 0)
        {
            g.ret(0);
            return;
        }
        link = node + 4;
    }
}

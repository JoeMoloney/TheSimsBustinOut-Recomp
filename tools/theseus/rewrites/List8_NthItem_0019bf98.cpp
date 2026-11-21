// The item (node +0) of node number a1 in the list headed at *a0 (next at +8); a1 <= 0 gives the
// first item, a list too short gives 0 (a1 compared on all 64 bits). 2 call sites.
#include "theseus.h"

void List8_NthItem_0019bf98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(0));
    if (node == 0)
    {
        g.ret(0);
        return;
    }
    const int64_t n = g.sarg64(1);
    for (int64_t i = 0; i < n; ++i)
    {
        node = g.read32(node + 8);
        if (node == 0)
        {
            g.ret(0);
            return;
        }
    }
    g.ret(g.read32(node));
}

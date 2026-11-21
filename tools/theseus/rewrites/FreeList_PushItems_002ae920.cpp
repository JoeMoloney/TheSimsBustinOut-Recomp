// Pushes a2 items of a1 bytes, starting at a3, onto the singly linked free list whose head is
// *a0 (link word at +0), last item first, so the list then runs a3, a3 + a1, ... Nothing for
// a3 == 0 or a2 <= 0. v0 is left as the original leaves it: untouched for a3 == 0, else the
// head read last (or (a2 - 1) * a1 when no item is pushed). 6 call sites.
#include "theseus.h"

void FreeList_PushItems_002ae920(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(3) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t head = g.arg(0), size = g.arg(1);
    int32_t i = static_cast<int32_t>(g.arg(2) - 1);
    uint32_t v0 = static_cast<uint32_t>(i) * size;
    uint32_t item = g.arg(3) + v0;
    for (; i >= 0; --i, item -= size)
    {
        v0 = g.read32(head);
        g.write32(item, v0);
        g.write32(head, item);
    }
    g.ret(v0);
}

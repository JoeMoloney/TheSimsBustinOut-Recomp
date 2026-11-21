// Appends node a1 to the doubly linked list at a0 (head +0x28, tail +0x2C; node next +0x178,
// prev +0x17C) unless it is already in it (searched from the head). Returns a1 if found, else
// the old tail. 4 call sites.
#include "theseus.h"

void DList28_AppendUnique_0026c7d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t list = g.arg(0) + 0x28, node = g.arg(1);
    const int64_t node64 = g.sarg64(1);
    for (uint32_t it = g.read32(list); it != 0; it = g.read32(it + 0x178))
    {
        if (static_cast<int64_t>(static_cast<int32_t>(it)) == node64)
        {
            g.ret(it);
            return;
        }
    }
    g.write32(node + 0x17C, g.read32(list + 4));
    const uint32_t tail = g.read32(list + 4);
    if (tail == 0)
        g.write32(list, node);
    else
        g.write32(tail + 0x178, node);
    g.write32(node + 0x178, 0);
    g.write32(list + 4, node);
    g.ret(tail);
}

// Appends node a1 to the doubly linked list at a0+0xC (head +0xC, tail +0x10; node prev +0, next +4), sets a0+8 = 1 and increments the count a0+0x18. Returns the new count (v1 = 1). 1 call site.
#include "theseus.h"

void DListC_AppendCount_002585a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t list = g.arg(0) + 0xC, node = g.arg(1);
    g.write32(node, g.read32(list + 4));
    const uint32_t tail = g.read32(list + 4);
    if (tail == 0)
        g.write32(g.arg(0) + 0xC, node);
    else
        g.write32(tail + 4, node);
    g.write32(node + 4, 0);
    g.write32(list + 4, node);
    g.write32(g.arg(0) + 8, 1);
    const uint32_t count = g.read32(g.arg(0) + 0x18) + 1;
    g.write32(g.arg(0) + 0x18, count);
    SET_GPR_S32(ctx, 3, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(count);
}

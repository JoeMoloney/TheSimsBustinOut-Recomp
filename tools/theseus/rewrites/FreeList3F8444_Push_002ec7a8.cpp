// Pushes a0 onto the free list whose head is the global 0x3F8444 ($gp - 0x76AC): *a0 = head, head = a0. Returns the old head. 2 call sites.
#include "theseus.h"

void FreeList3F8444_Push_002ec7a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t old = g.read32(0x3F8444);
    g.write32(g.arg(0), old);
    g.write32(0x3F8444, g.arg(0));
    g.ret(old);
}

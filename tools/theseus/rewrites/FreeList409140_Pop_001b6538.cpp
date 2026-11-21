// Pops the first node of the free list whose head is the global 0x409140 (link +0); returns it, or 0 when the list is empty. 1 call site.
#include "theseus.h"

void FreeList409140_Pop_001b6538(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.read32(0x409140);
    if (head != 0)
        g.write32(0x409140, g.read32(head));
    SET_GPR_S32(ctx, 4, 0x410000);  // a0, as the original leaves it; gate: equivalent
    g.ret(head);
}

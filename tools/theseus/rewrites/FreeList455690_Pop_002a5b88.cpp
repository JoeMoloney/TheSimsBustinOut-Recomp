// Pops the first node of the free list whose head is the global 0x455690 (link +0); returns it, or 0 when empty (a0 = 0x450000). 1 call site.
#include "theseus.h"

void FreeList455690_Pop_002a5b88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.read32(0x455690);
    if (head != 0)
        g.write32(0x455690, g.read32(head));
    SET_GPR_S32(ctx, 4, 0x450000);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(head);
}

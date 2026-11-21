// Pushes a0 (when nonzero) onto the free list whose head is the global 0x456570: *a0 = head, head = a0; returns the old head (v0 untouched for a0 == 0). 2 call sites.
#include "theseus.h"

void FreeList456570_PushNonNull_002d4488(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t old = g.read32(0x456570);
    g.write32(g.arg(0), old);
    g.write32(0x456570, g.arg(0));
    g.ret(old);
}

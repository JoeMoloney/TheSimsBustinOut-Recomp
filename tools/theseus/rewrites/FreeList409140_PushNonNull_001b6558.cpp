// Pushes a0 (when nonzero) onto the free list whose head is the global 0x409140 (link +0); returns the old head (v0 untouched for a0 == 0). v1 = 0x410000 (the `lui` in the delay slot). 1 call site.
#include "theseus.h"

void FreeList409140_PushNonNull_001b6558(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    SET_GPR_S32(ctx, 3, 0x410000);  // scratch register for callers (not ABI-compared); gate: equivalent
    if (g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t old = g.read32(0x409140);
    g.write32(g.arg(0), old);
    g.write32(0x409140, g.arg(0));
    g.ret(old);
}

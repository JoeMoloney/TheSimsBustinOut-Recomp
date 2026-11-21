// Pushes a0 (when nonzero) onto the free list whose head is the global 0x3FA878 (link +0); returns the old head (v0 untouched for a0 == 0). 1 call site.
#include "theseus.h"

void FreeList3FA878_PushNonNull_00170f78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t old = g.read32(0x3FA878);
    g.write32(g.arg(0), old);
    g.write32(0x3FA878, g.arg(0));
    g.ret(old);
}

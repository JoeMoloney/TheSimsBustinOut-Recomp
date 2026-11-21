// Pushes a0 (when nonzero) onto the free list whose head is the global 0x44DF20 (link +0); returns the old head (v0 untouched for a0 == 0). v1 = 0x450000. 1 call site.
#include "theseus.h"

void FreeList44DF20_PushNonNull_0026d928(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    SET_GPR_S32(ctx, 3, 0x450000);  // scratch register for callers (not ABI-compared); gate: equivalent
    if (g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t old = g.read32(0x44DF20);
    g.write32(g.arg(0), old);
    g.write32(0x44DF20, g.arg(0));
    g.ret(old);
}

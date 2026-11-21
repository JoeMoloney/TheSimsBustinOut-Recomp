// Pushes a0 (when nonzero) onto the free list at +0x650 of the object [0x3F9248] (link +0); returns the old head (v0 untouched for a0 == 0). v1 = the object. 1 call site.
#include "theseus.h"

void Pool3F9248_FreeNonNull_002fd810(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(0x3F9248);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(obj));  // scratch register for callers (not ABI-compared); gate: equivalent
    if (g.arg64(0) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t old = g.read32(obj + 0x650);
    g.write32(g.arg(0), old);
    g.write32(obj + 0x650, g.arg(0));
    g.ret(old);
}

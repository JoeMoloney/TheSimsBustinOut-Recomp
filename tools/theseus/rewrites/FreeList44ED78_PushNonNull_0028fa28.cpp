// Pushes a0 (when nonzero) onto the free list whose head is the global 0x44ED78 (block 0x44ED70 + 8; link +0). v0 = 0x44ED70 and v1 = the old head, or v0 = 0x450000 for a0 == 0. 1 call site.
#include "theseus.h"

void FreeList44ED78_PushNonNull_0028fa28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) == 0)
    {
        g.ret(0x450000);
        return;
    }
    const uint32_t old = g.read32(0x44ED78);
    g.write32(g.arg(0), old);
    g.write32(0x44ED78, g.arg(0));
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(old));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x44ED70);
}

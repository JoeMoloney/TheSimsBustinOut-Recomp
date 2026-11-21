// Stores a1 rounded up to a multiple of 32 at a0+0xC. v0 = the mask 0xFFFFFFE0. 1 call site.
#include "theseus.h"

void FieldC_SetAligned32_002ac4e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0xC, (g.arg(1) + 0x1F) & ~0x1Fu);
    g.ret(0xFFFFFFE0u);
}

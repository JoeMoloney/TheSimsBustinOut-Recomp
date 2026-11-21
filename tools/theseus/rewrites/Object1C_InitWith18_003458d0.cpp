// Initialiser: +0x18 = a1, +8, +0xC, +0x14, +0, +4 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object1C_InitWith18_003458d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x18, g.arg(1));
    g.write32(p + 8, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x14, 0);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.ret_copy_of_arg(0);
}

// Initialiser: +0 = a1, +4..+0x18 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object1C_InitFirst_0035d190(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x10, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 0x18, 0);
    g.ret_copy_of_arg(0);
}

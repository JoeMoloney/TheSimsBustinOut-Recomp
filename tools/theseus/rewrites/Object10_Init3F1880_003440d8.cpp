// Initialiser: +0xC = 0x3F8588, +0 = 0x3F1880 (vtable), +4 = +8 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object10_Init3F1880_003440d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, 0x3F8588);
    g.write32(p, 0x3F1880);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.ret_copy_of_arg(0);
}

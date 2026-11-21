// Initialiser: +0 = 0, +0x18 = 0x3E4050, +0x1C = 0, +0x20 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object_Init3E4050_002ada68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0);
    g.write32(p + 0x18, 0x3E4050);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x20, 0);
    g.ret_copy_of_arg(0);
}

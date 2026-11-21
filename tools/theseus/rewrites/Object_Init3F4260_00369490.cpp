// Initialiser: +8 = 0, +0 = 0x3F4260 (vtable), +4 = 0, +0xC = 0. Returns a0. 2 call sites.
#include "theseus.h"

void Object_Init3F4260_00369490(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 0);
    g.write32(p, 0x3F4260);
    g.write32(p + 4, 0);
    g.write32(p + 0xC, 0);
    g.ret_copy_of_arg(0);
}

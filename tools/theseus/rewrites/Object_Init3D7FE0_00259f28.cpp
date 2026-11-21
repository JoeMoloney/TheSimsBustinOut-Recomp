// Initialiser: +4 = 0, +0 = 0x3D7FE0 (vtable), +8 = 0. Returns a0. 2 call sites.
#include "theseus.h"

void Object_Init3D7FE0_00259f28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0) + 0, 0x3D7FE0);
    g.write32(g.arg(0) + 8, 0);
    g.ret_copy_of_arg(0);
}

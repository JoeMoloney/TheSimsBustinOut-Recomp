// Stores 0x3D7828 (a vtable) at a0+0x18; returns a0. 1 call site.
#include "theseus.h"

void Field18_SetVtable3D7828_002580c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x18, 0x3D7828);
    g.ret_copy_of_arg(0);
}

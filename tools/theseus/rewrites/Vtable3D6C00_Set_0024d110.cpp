// Stores 0x3D6C00 (a vtable) at *a0; returns a0. 1 call site.
#include "theseus.h"

void Vtable3D6C00_Set_0024d110(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x3D6C00);
    g.ret_copy_of_arg(0);
}

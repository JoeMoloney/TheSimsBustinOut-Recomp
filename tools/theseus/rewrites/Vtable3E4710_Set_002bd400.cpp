// Stores 0x3E4710 (a vtable) at *a0 and returns a0. 2 call sites.
#include "theseus.h"

void Vtable3E4710_Set_002bd400(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x3E4710);
    g.ret_copy_of_arg(0);
}

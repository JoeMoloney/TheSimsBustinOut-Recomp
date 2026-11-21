// Stores 0x3CAFC8 (a vtable) at a0+0x5C and returns a0. 2 call sites.
#include "theseus.h"

void Field5C_SetVtable3CAFC8_002635d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x5C, 0x3CAFC8);
    g.ret_copy_of_arg(0);
}

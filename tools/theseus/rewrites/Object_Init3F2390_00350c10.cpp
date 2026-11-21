// Small constructor: +4 = a1, +0 = 0x3F2390 (vtable). Returns a0. 2 call sites.
#include "theseus.h"

void Object_Init3F2390_00350c10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, g.arg(1));
    g.write32(g.arg(0), 0x3F2390);
    g.ret_copy_of_arg(0);
}

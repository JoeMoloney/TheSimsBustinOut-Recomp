// Constructor: +4 = 0, +0 = 0x3E6688 (vtable), +8 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object_Init3E6688_002c6f98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0), 0x3E6688);
    g.write32(g.arg(0) + 8, 0);
    g.ret_copy_of_arg(0);
}

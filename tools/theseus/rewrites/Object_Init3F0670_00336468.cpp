// Constructor: +4 = 0, +0 = 0x3F0670 (vtable), +8 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object_Init3F0670_00336468(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0), 0x3F0670);
    g.write32(g.arg(0) + 8, 0);
    g.ret_copy_of_arg(0);
}

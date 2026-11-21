// Zeroes the words at a0+0, +0x4488, +0x4304; returns a0. 1 call site.
#include "theseus.h"

void Fields_Clear0_4304_4488_00232488(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0);
    g.write32(g.arg(0) + 0x4488, 0);
    g.write32(g.arg(0) + 0x4304, 0);
    g.ret_copy_of_arg(0);
}

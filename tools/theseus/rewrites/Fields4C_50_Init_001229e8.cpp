// a0+0x4C = 0, +0x50 = 1; returns a0. 1 call site. (The map joined another function'\''s epilogue after the `jr $ra`.)
#include "theseus.h"

void Fields4C_50_Init_001229e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x4C, 0);
    g.write32(g.arg(0) + 0x50, 1);
    g.ret_copy_of_arg(0);
}

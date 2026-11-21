// Initialiser: +4 = 0, +0 = 4, +8 = 0. Returns a0. 1 call site. (The map joined another function'\''s epilogue after the `jr $ra`.)
#include "theseus.h"

void Triple_Init400_0035a5a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0), 4);
    g.write32(g.arg(0) + 8, 0);
    g.ret_copy_of_arg(0);
}

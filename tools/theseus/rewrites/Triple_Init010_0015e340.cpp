// Initialiser: +4 = 0, +8 = 1, +0 = 0. Returns a0. 1 call site.
#include "theseus.h"

void Triple_Init010_0015e340(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0) + 8, 1);
    g.write32(g.arg(0), 0);
    g.ret_copy_of_arg(0);
}

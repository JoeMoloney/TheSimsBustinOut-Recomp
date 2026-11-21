// Clears word a1 of the array at a0+0x10. 11 call sites.
#include "theseus.h"

void Words10_ClearAt_002581a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + (g.arg(1) << 2) + 0x10, 0);
    g.ret_void();
}

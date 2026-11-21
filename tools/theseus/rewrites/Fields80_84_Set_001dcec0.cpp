// Stores a2 at a0+0x84 and a1 at a0+0x80. 5 call sites.
#include "theseus.h"

void Fields80_84_Set_001dcec0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x84, g.arg(2));
    g.write32(g.arg(0) + 0x80, g.arg(1));
    g.ret_void();
}

// Of the three words at a0+0x78/0x7C/0x80, sets +0x80 = a1 and clears the other two. 18 call sites.
#include "theseus.h"

void Slots78_Set80_001f9538(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x80, g.arg(1));
    g.write32(g.arg(0) + 0x78, 0);
    g.write32(g.arg(0) + 0x7C, 0);
    g.ret_void();
}

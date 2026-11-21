// Of the three words at a0+0x78/0x7C/0x80, sets +0x78 = a1 and clears the other two. 28 call sites.
#include "theseus.h"

void Slots78_Set78_001f9518(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x78, g.arg(1));
    g.write32(g.arg(0) + 0x80, 0);
    g.write32(g.arg(0) + 0x7C, 0);
    g.ret_void();
}

// Of the three words at a0+0x78/0x7C/0x80, sets +0x7C = a1 and clears the other two. 20 call sites.
#include "theseus.h"

void Slots78_Set7C_001f9528(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x7C, g.arg(1));
    g.write32(g.arg(0) + 0x80, 0);
    g.write32(g.arg(0) + 0x78, 0);
    g.ret_void();
}

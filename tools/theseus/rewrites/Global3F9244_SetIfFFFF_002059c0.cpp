// Sets the global word 0x3F9244 to 1 when a1 == 0xFFFF and a0 != 0 (whole registers). v0 = 1 when a1 == 0xFFFF, else 0xFFFF. 2 call sites.
#include "theseus.h"

void Global3F9244_SetIfFFFF_002059c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF)
    {
        g.ret(0xFFFF);
        return;
    }
    if (g.arg64(0) != 0)
        g.write32(0x3F9244, 1);
    g.ret(1);
}

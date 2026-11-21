// Zeroes the four globals 0x3F8E20..0x3F8E2C when a1 == 0xFFFF and a0 != 0 (whole registers). Returns 0xFFFF. 2 call sites.
#include "theseus.h"

void Globals3F8E20_ClearIfFFFF_00227fb0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) == 0xFFFF && g.arg64(0) != 0)
    {
        g.write32(0x3F8E20, 0);
        g.write32(0x3F8E24, 0);
        g.write32(0x3F8E28, 0);
        g.write32(0x3F8E2C, 0);
    }
    g.ret(0xFFFF);
}

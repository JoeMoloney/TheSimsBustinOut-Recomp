// When a1 == 0xFFFF (whole register): sets the four table pointers 0x3F8AE0/E8/F0/F8 ($gp - 0x7010.. - 0x6FF8) to 0x3C1550/0x3C1530/0x3C1510/0x3C14F0 when a0 != 0, else all four to 0x3B3498. v0 = 0xFFFF when a1 != 0xFFFF, else the first value stored. 2 call sites.
#include "theseus.h"

void Globals3F8AE0_SetTablesIfFFFF_001b1748(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF)
    {
        g.ret(0xFFFF);
        return;
    }
    if (g.arg64(0) == 0)
    {
        g.write32(0x3F8AE0, 0x3B3498);
        g.write32(0x3F8AF8, 0x3B3498);
        g.write32(0x3F8AF0, 0x3B3498);
        g.write32(0x3F8AE8, 0x3B3498);
        g.ret(0x3B3498);
        return;
    }
    g.write32(0x3F8AE0, 0x3C1550);
    g.write32(0x3F8AE8, 0x3C1530);
    g.write32(0x3F8AF0, 0x3C1510);
    g.write32(0x3F8AF8, 0x3C14F0);
    g.ret(0x3C1550);
}

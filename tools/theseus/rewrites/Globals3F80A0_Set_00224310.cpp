// Sets the globals 0x3F80A0 = a0, 0x3F80A4 = a1, 0x3F80A8 = 0x3F80AC = 0 ($gp - 0x7A50..). 1 call site.
#include "theseus.h"

void Globals3F80A0_Set_00224310(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F80A0, g.arg(0));
    g.write32(0x3F80A4, g.arg(1));
    g.write32(0x3F80A8, 0);
    g.write32(0x3F80AC, 0);
    g.ret_void();
}

// Sets the global block 0x44A9D8: +0 = a0, +4 = a0, +8 = a1, +0x10 = 0. v0 = 0x450000, v1 = 0x44A9D8. 1 call site. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Globals44A9D8_Set_00233ba0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x44A9D8, g.arg(0));
    g.write32(0x44A9DC, g.arg(0));
    g.write32(0x44A9E0, g.arg(1));
    g.write32(0x44A9E8, 0);
    SET_GPR_S32(ctx, 3, 0x44A9D8);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x450000);
}

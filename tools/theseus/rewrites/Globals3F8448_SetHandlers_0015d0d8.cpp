// Sets the globals 0x3F8448 = 0x15D098 and 0x3F844C = 0x15D0B8 (two function pointers). v0/v1 = those values. 1 call site.
#include "theseus.h"

void Globals3F8448_SetHandlers_0015d0d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F8448, 0x15D098);
    g.write32(0x3F844C, 0x15D0B8);
    SET_GPR_S32(ctx, 3, 0x15D0B8);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x15D098);
}

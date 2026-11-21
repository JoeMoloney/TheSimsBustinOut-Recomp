// Sets the global flag 0x3F7BD8 ($gp - 0x7F18) to 1 if it is 0 (no store otherwise); returns 1. 1 call site.
#include "theseus.h"

void Flag3F7BD8_SetOnce_00136e40(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x3F7BD8) == 0)
        g.write32(0x3F7BD8, 1);
    g.ret(1);
}

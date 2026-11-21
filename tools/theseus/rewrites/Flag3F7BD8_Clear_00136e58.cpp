// Zeroes the global flag 0x3F7BD8 ($gp - 0x7F18). 1 call site. Pairs with Flag3F7BD8_SetOnce_00136e40. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Flag3F7BD8_Clear_00136e58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F7BD8, 0);
    g.ret_void();
}

// Returns 1. 3 call sites. (The map joined two other functions after this `jr $ra`: vector getters at +0x2D4.. and a switch on a1.)
#include "theseus.h"

void ReturnOne_001af2c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(1);
}

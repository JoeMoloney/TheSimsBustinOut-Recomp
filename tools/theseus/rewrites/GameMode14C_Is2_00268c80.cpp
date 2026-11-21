// 1 if the mode [[0x3F80B8]+0x14C] is 2, else 0. 1 call site.
#include "theseus.h"

void GameMode14C_Is2_00268c80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(0x003F80B8) + 0x14C) == 2 ? 1 : 0);
}

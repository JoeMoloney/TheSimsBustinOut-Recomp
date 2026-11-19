// Returns the global at $gp - 0x75B4 (0x3F853C with the game's $gp). 14 call sites.
#include "theseus.h"

void Global_gpm75b4_0033d2a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.gp() - 0x75B4));
}

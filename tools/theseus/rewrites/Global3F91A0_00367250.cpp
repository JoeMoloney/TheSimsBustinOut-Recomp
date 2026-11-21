// The word at the global 0x3F91A0. 1 call site.
#include "theseus.h"

void Global3F91A0_00367250(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x003F91A0));
}

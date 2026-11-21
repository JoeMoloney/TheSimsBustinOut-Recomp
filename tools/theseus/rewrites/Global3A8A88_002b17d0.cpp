// The word at the global 0x3A8A88. 1 call site.
#include "theseus.h"

void Global3A8A88_002b17d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x003A8A88));
}

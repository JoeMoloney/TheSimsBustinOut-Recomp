// Sets the global word 0x3F8DF8 to 1 and returns 1. 4 call sites.
#include "theseus.h"

void Global3F8DF8_SetOne_0020d3e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F8DF8, 1);
    g.ret(1);
}

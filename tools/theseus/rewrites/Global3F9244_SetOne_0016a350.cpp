// Sets the global word 0x3F9244 to 1 and returns 1. 7 call sites (2 identical copies).
// theseus: copies 0x1bca50
#include "theseus.h"

void Global3F9244_SetOne_0016a350(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F9244, 1);
    g.ret(1);
}

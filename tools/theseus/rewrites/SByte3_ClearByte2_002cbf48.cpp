// Clears the byte at a0+2 and returns a0 (all 64 bits). 27 call sites.
#include "theseus.h"

void SByte3_ClearByte2_002cbf48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0) + 2, 0);
    g.ret_copy_of_arg(0);
}

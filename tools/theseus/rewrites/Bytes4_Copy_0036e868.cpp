// Copies 4 bytes from a1 to a0, one at a time. Returns a0. 2 call sites.
#include "theseus.h"

void Bytes4_Copy_0036e868(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t i = 0; i < 4; ++i)
        g.write8(g.arg(0) + i, g.read8(g.arg(1) + i));  // gate: equivalent read16: write8 keeps only the low byte
    g.ret_copy_of_arg(0);
}

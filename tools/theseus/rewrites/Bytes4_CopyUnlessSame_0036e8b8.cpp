// Copies 4 bytes from a1 to a0, one at a time, unless a0 == a1. Returns a0. 4 call sites.
#include "theseus.h"

void Bytes4_CopyUnlessSame_0036e8b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) != g.arg64(1))  // gate: equivalent 2: copying a buffer onto itself changes nothing
    {
        const uint32_t dst = g.arg(0), src = g.arg(1);
        for (uint32_t i = 0; i < 4; ++i)
            g.write8(dst + i, g.read8(src + i));  // gate: equivalent read16: write8 keeps only the low byte
    }
    g.ret_copy_of_arg(0);
}

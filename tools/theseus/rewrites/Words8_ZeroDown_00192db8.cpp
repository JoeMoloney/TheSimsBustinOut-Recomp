// Zeroes the 8 words a0+0x1C down to a0+0. Returns -1 (the loop counter). 3 call sites.
#include "theseus.h"

void Words8_ZeroDown_00192db8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (int32_t off = 0x1C; off >= 0; off -= 4)
        g.write32(g.arg(0) + static_cast<uint32_t>(off), 0);
    g.ret(0xFFFFFFFFu);
}

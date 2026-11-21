// The word a1 of the array at a0+0x88 (written as (a1 + 10) * 4 + 0x60). 1 call site.
#include "theseus.h"

void Words88_At_00361578(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + ((g.arg(1) + 10) << 2) + 0x60));
}

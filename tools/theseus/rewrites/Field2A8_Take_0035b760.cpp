// Returns the word at a0+0x2A8 and zeroes it if it was nonzero (no store otherwise). 3 call sites.
#include "theseus.h"

void Field2A8_Take_0035b760(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x2A8, value = g.read32(field);
    if (value != 0)
        g.write32(field, 0);
    g.ret(value);
}

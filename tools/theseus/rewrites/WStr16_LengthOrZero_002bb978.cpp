// Number of 16-bit characters before the terminating 0 at a0; 0 for a null pointer
// (all 64 bits of a0 are tested). 5 call sites.
#include "theseus.h"

void WStr16_LengthOrZero_002bb978(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t s = g.arg(0);
    uint32_t n = 0;
    while (g.read16(s + 2 * n) != 0)
        ++n;
    g.ret(n);
}

// Number of 32-bit characters before the terminating 0 at a0 (3 call sites).
#include "theseus.h"

void WStr32_Length_00349bd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0);
    uint32_t n = 0;
    while (g.read32(s + 4 * n) != 0)
        ++n;
    g.ret(n);
}

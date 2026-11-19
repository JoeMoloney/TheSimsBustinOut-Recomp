// Number of 16-bit characters before the terminating 0 (wide strlen; 21 call sites).
#include "theseus.h"

void WStr16_Length_00349b98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0);
    uint32_t n = 0;
    while (g.read16(s + 2 * n) != 0)
        ++n;
    g.ret(n);
}

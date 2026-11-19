// Length of the list whose head pointer is at a0 (next pointer at +8). 3 call sites.
#include "theseus.h"

void List8_Length_002b3cd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t n = 0;
    for (uint32_t node = g.read32(g.arg(0)); node != 0; node = g.read32(node + 8))
        ++n;
    g.ret(n);
}

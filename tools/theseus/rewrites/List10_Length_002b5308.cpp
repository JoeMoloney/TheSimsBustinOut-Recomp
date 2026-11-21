// The number of nodes in the list headed at *a0 (next at +0x10). 1 call site.
#include "theseus.h"

void List10_Length_002b5308(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t count = 0;
    for (uint32_t node = g.read32(g.arg(0)); node != 0; node = g.read32(node + 0x10))
        ++count;
    g.ret(count);
}

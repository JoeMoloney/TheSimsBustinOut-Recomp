// The number of nodes with a non-zero value (+0) in the list from [a0+0x14] (next at +8). 1 call site.
#include "theseus.h"

void List8_CountNonNull_001542e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t count = 0;
    for (uint32_t node = g.read32(g.arg(0) + 0x14); node != 0; node = g.read32(node + 8))
        if (g.read32(node) != 0)
            ++count;
    g.ret(count);
}

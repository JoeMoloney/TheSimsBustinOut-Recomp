// The sum of the words at +4 of the [a0+0x24] 16-byte entries of the array [a0+0x20] (0 if the
// count is not positive). 1 call site.
#include "theseus.h"

void Array16_SumField4_0027c808(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 0x24));
    uint32_t sum = 0;
    if (count > 0)  // gate: equivalent >=: a count of 0 runs no iterations either way
        for (uint32_t p = g.read32(g.arg(0) + 0x20) + 4; count != 0; --count, p += 16)
            sum += g.read32(p);
    g.ret(sum);
}

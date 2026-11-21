// Zeroes the [a0+4] halfwords of the array [a0+8] (count and base re-read every step). Returns 0, or the count when it is <= 0 (32-bit test, as the generated code does). 1 call site.
#include "theseus.h"

void Halves_ZeroCounted_0031de38(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 4));
    if (count <= 0)
    {
        g.ret(static_cast<uint32_t>(count));
        return;
    }
    int32_t i = 0;
    do
    {
        g.write16(g.read32(g.arg(0) + 8) + (static_cast<uint32_t>(i) << 1), 0);
        ++i;
    } while (i < static_cast<int32_t>(g.read32(g.arg(0) + 4)));
    g.ret(0);
}

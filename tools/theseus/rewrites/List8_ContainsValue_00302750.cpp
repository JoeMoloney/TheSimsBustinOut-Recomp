// 1 if a0 (all 64 bits) is the value (+0) of a node in the list headed at *a1 (next at +8),
// else 0. 1 call site.
#include "theseus.h"

void List8_ContainsValue_00302750(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t value = g.sarg64(0);
    for (uint32_t node = g.read32(g.arg(1)); node != 0; node = g.read32(node + 8))
        if (static_cast<int32_t>(g.read32(node)) == value)
        {
            g.ret(1);
            return;
        }
    g.ret(0);
}

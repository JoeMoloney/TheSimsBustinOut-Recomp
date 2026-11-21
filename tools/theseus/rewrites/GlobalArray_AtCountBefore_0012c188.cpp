// The global at 0x3FA5D0 points to a holder whose +0 is an array of pointers with its count in
// the word before it (null array = count 0). Returns *array[a0] if a0 < count (signed, all 64
// bits of a0), else 0. 2 call sites.
#include "theseus.h"

void GlobalArray_AtCountBefore_0012c188(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t holder = g.read32(0x003FA5D0);
    const uint32_t array = g.read32(holder);
    const int64_t count = array == 0 ? 0 : static_cast<int32_t>(g.read32(array - 4));
    if (g.sarg64(0) < count)
    {
        const uint32_t element = g.read32(g.read32(holder) + (g.arg(0) << 2));
        g.ret(g.read32(element));
        return;
    }
    g.ret(0);
}

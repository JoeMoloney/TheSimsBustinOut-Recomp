// The global 0x3FA5D4 points to a holder whose +0 is a pointer array with its count in the word
// before it (null array = count 0). Returns *array[a0] if a0 < count - 1 (signed, all 64 bits of
// a0), else 0. 1 call site.
#include "theseus.h"

void GlobalArray3FA5D4_AtBelowLast_0012e318(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t holder = g.read32(0x003FA5D4);
    const uint32_t array = g.read32(holder);
    const int32_t count = array == 0 ? 0 : static_cast<int32_t>(g.read32(array - 4));
    if (g.sarg64(0) < static_cast<int32_t>(static_cast<uint32_t>(count) - 1))
    {
        const uint32_t element = g.read32(g.read32(holder) + (g.arg(0) << 2));
        g.ret(g.read32(element));
        return;
    }
    g.ret(0);
}

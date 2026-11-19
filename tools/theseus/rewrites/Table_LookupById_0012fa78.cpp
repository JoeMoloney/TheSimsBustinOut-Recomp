// Looks up id a0 (all 64 bits) in the pointer array that the object at the global 0x3FA5D8
// holds at +0 (element count stored just before the array): returns word +4 of the first
// element whose word +0xC matches, or 10 if none (3 call sites).
#include "theseus.h"

void Table_LookupById_0012fa78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t owner = g.read32(0x3FA5D8);
    const uint32_t array = g.read32(owner);
    const int32_t count = array != 0 ? static_cast<int32_t>(g.read32(array - 4)) : 0;  // gate: equivalent 1: array 1 is never ruled
    for (int32_t i = 0; i < count; ++i)
    {
        const uint32_t elem = g.read32(array + 4 * i);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(elem + 0xC))) == g.sarg64(0))
        {
            g.ret(g.read32(elem + 4));
            return;
        }
    }
    g.ret(10);
}

// [a0+0xC4] points to a holder whose +0 is a word array with its count in the word before it
// (null array = count 0). Returns the index of the first element equal to a1 (all 64 bits of
// a1), or 0 when there is none (index 0 and "not found" look the same). 2 call sites.
#include "theseus.h"

void CountedArray_IndexOf_001644f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t holder = g.read32(g.arg(0) + 0xC4);
    const uint32_t array = g.read32(holder);
    const int32_t count = array == 0 ? 0 : static_cast<int32_t>(g.read32(array - 4));  // gate: equivalent 1: a count of 1 checks only index 0, which returns 0 like "not found"
    const int64_t key = g.sarg64(1);
    for (int32_t i = 0; i < count; ++i)
        if (static_cast<int32_t>(g.read32(array + 4 * i)) == key)
        {
            g.ret(i);
            return;
        }
    g.ret(0);
}

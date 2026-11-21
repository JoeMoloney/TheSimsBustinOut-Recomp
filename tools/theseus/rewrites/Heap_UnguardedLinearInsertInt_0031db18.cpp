// std::__unguarded_linear_insert(int *last = a0, int val = a1): shifts the elements before last up by one while val < the element (signed, val as a whole register), then stores val in the gap. No lower bound (the caller guarantees a smaller element). Returns 0 (the last compare). 2 call sites.
#include "theseus.h"

void Heap_UnguardedLinearInsertInt_0031db18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t val = g.sarg64(1);
    uint32_t last = g.arg(0);
    while (val < static_cast<int32_t>(g.read32(last - 4)))
    {
        g.write32(last, g.read32(last - 4));
        last -= 4;
    }
    g.write32(last, g.arg(1));
    g.ret(0);
}

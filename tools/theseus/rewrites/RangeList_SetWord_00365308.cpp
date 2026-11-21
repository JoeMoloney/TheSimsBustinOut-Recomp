// Walks a list of index ranges from node a0 (+4 count, +8 first index, +0xC word array,
// +0x10 next) to the node with first <= a1 <= first + count - 1 (signed), stores a2 in its
// array at a1 - first and returns 1. There is no end-of-list test: callers know the index is
// covered. 7 call sites.
#include "theseus.h"

void RangeList_SetWord_00365308(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t index = g.sarg64(1);  // `slt` compares all 64 bits
    uint32_t node = g.arg(0);
    int32_t first;
    for (;;)
    {
        first = static_cast<int32_t>(g.read32(node + 8));
        const int32_t last = static_cast<int32_t>(static_cast<uint32_t>(first) + g.read32(node + 4) - 1);
        if (index >= first && index <= last)
            break;
        node = g.read32(node + 0x10);
    }
    g.write32(g.read32(node + 0xC) + ((g.arg(1) - static_cast<uint32_t>(first)) << 2), g.arg(2));
    g.ret(1);
}

// Binary search tree under the root at +8 of a0 (node: +0 left, +4 right, +0x18 key; the shared
// nil node 0x3A8AA8 ends a branch): the last node visited when descending for a1 (left if
// a1 < key, else right; unsigned, a1 on all 64 bits), i.e. where a1 would be attached; 0 for an
// empty tree. 1 call site.
#include "theseus.h"

void Tree18_InsertParent_002b4bd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    constexpr uint32_t kNil = 0x3A8AA8;
    const uint64_t key = g.arg64(1);
    uint32_t last = 0;
    for (uint32_t node = g.read32(g.arg(0) + 8); node != kNil;)
    {
        const uint64_t here = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(node + 0x18))));
        last = node;
        node = g.read32(node + (key < here ? 0 : 4));
    }
    g.ret(last);
}

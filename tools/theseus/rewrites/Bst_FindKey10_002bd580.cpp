// Binary search tree from the global 0x3A8B08 (key +0x10, left +0x1C, right +0x20): the node whose
// key equals a0, or 0 (unsigned compares; a0 on all 64 bits). 1 call site.
#include "theseus.h"

void Bst_FindKey10_002bd580(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t key = g.arg64(0);
    uint32_t node = g.read32(0x003A8B08);
    while (node != 0)
    {
        const uint64_t k = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(node + 0x10))));
        if (key < k)
            node = g.read32(node + 0x1C);
        else if (k < key)
            node = g.read32(node + 0x20);
        else
            break;
    }
    g.ret(node);
}

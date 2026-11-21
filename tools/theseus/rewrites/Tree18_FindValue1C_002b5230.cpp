// Binary search tree under the root at +8 of a0 (node: +0 left, +4 right, +0x18 key, +0x1C value;
// the shared nil node 0x3A8AA8 ends a branch): the node whose key equals a1 (unsigned, a1 on all
// 64 bits), or 0. When found and a2 (all 64 bits) is not null, *a2 = the node's value. 50 call sites.
#include "theseus.h"

void Tree18_FindValue1C_002b5230(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    constexpr uint32_t kNil = 0x3A8AA8;
    const uint64_t key = g.arg64(1);
    for (uint32_t node = g.read32(g.arg(0) + 8); node != kNil;)
    {
        const uint64_t here = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(node + 0x18))));
        if (key == here)
        {
            if (g.arg64(2) != 0)
                g.write32(g.arg(2), g.read32(node + 0x1C));
            g.ret(node);
            return;
        }
        node = g.read32(node + (key < here ? 0 : 4));  // gate: equivalent <=: key == here returned above
    }
    g.ret(0);
}

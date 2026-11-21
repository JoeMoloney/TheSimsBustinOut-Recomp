// Hash table lookup: bucket a1 of the bucket array at a0+8, then the chain (next at +8) for the
// first node whose key at +0xC equals a2 (all 64 bits of a2); returns the node or 0. 2 call sites.
#include "theseus.h"

void HashChain_FindKeyC_002b0218(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(2);
    const uint32_t buckets = g.read32(g.arg(0) + 8);
    for (uint32_t node = g.read32(buckets + (g.arg(1) << 2)); node != 0; node = g.read32(node + 8))
        if (static_cast<int32_t>(g.read32(node + 0xC)) == key)
        {
            g.ret(node);
            return;
        }
    g.ret(0);
}

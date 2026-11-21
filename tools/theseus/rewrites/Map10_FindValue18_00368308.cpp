// std::map<unsigned, T>::find on the map whose header is [a0+4] (root at header+4; node: left
// +8, right +0xC, key +0x10, value +0x18): returns the value of the node with key a1, or 0.
// 4 call sites. (The original also spills a1 and the result to its stack frame.)
#include "theseus.h"

void Map10_FindValue18_00368308(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t header = g.read32(g.arg(0) + 4);
    const uint64_t key64 = g.arg64(1);  // the walk compares the whole register (`sltu`)
    uint32_t lower = header;
    for (uint32_t node = g.read32(header + 4); node != 0;)
    {
        const uint64_t node_key = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(node + 0x10))));
        if (node_key < key64)
            node = g.read32(node + 0xC);
        else
        {
            lower = node;
            node = g.read32(node + 8);
        }
    }
    const uint32_t end = g.read32(g.arg(0) + 4);
    // the final test reloads a1 from the stack (`lw`): its low 32 bits
    if (lower == end || g.arg(1) < g.read32(lower + 0x10))
    {
        g.ret(0);
        return;
    }
    g.ret(g.read32(lower + 0x18));
}

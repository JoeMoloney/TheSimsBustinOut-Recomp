// Walks the list from [a0+0x98] (next at +4) for the first node whose word at +0x10 equals a1
// (all 64 bits of a1); returns the node or 0. 1 call site.
#include "theseus.h"

void List4_FindField10_00290580(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    uint32_t node = g.read32(g.arg(0) + 0x98);
    while (node != 0 && static_cast<int32_t>(g.read32(node + 0x10)) != key)
        node = g.read32(node + 4);
    g.ret(node);
}

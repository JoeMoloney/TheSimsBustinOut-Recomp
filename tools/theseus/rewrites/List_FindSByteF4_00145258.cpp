// Walks the list whose head is *[a0+0x11C] (next at +0xF8) for the first node whose signed byte
// at +0xF4 equals a1 (all 64 bits of a1); returns the node or 0. 1 call site.
#include "theseus.h"

void List_FindSByteF4_00145258(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);
    uint32_t node = g.read32(g.read32(g.arg(0) + 0x11C));
    while (node != 0 && g.sread8(node + 0xF4) != key)
        node = g.read32(node + 0xF8);
    g.ret(node);
}

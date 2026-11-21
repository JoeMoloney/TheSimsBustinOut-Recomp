// Removes a0 from the singly linked free list whose head is the global 0x3F8444 (link at +0). Returns a0'\''s next if found, else 0. 2 call sites. Pairs with FreeList3F8444_Push_002ec7a8.
#include "theseus.h"

void FreeList3F8444_Remove_002ec7c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t target = g.sarg64(0);  // compared as a whole register
    uint32_t link = 0x3F8444;
    for (uint32_t node = g.read32(link); node != 0; node = g.read32(link))
    {
        if (static_cast<int64_t>(static_cast<int32_t>(node)) == target)
        {
            const uint32_t next = g.read32(node);
            g.write32(link, next);
            g.ret(next);
            return;
        }
        link = node;
    }
    g.ret(0);
}

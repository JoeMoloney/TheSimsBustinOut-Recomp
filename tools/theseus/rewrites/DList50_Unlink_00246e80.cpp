// Unlinks a0 from a doubly linked list (+0x50 prev, +0x54 next): prev->next = next, next->prev = prev (each when present; next is re-read after the first store), then clears both links. Returns a0. 3 call sites.
#include "theseus.h"

void DList50_Unlink_00246e80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t node = g.arg(0), prev = g.read32(node + 0x50);
    if (prev != 0)
        g.write32(prev + 0x54, g.read32(node + 0x54));
    const uint32_t next = g.read32(node + 0x54);
    if (next != 0)
        g.write32(next + 0x50, g.read32(node + 0x50));
    g.write32(node + 0x50, 0);
    g.write32(node + 0x54, 0);
    g.ret_copy_of_arg(0);
}

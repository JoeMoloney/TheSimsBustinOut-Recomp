// Allocates a0 bytes aligned to a1 from the current heap: heap = 0x268430(), then
// 0x2B0908(heap, size, align, 0). v0 is the allocator's. 284 call sites.
#include "theseus.h"

void Heap_AllocAligned_00268530(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t size = g.arg64(0), align = g.arg64(1);
    g.call(0x268430, 0x26854C);
    g.set_arg64(0, GPR_U64(g.ctx, 2));  // heap
    g.set_arg64(1, size);
    g.set_arg64(2, align);
    g.set_arg64(3, 0);
    g.call(0x2B0908, 0x268560);
    g.ret_void();
}

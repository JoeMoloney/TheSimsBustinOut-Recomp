// Frees the block a0: heap = 0x268430(), then 0x2B1098(heap, block). v0 is the callee's.
// (The allocation side is 0x268530 -> 0x2B0908(heap, size, align, 0).) 281 call sites.
#include "theseus.h"

void Heap_Free_00268500(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t block = g.arg64(0);
    g.call(0x268430, 0x268514);         // the heap to use
    g.set_arg64(0, GPR_U64(g.ctx, 2));  // daddu a0, v0, $zero
    g.set_arg64(1, block);              // daddu a1, s0, $zero
    g.call(0x2B1098, 0x268520);
    g.ret_void();  // v0 is the callee's
}

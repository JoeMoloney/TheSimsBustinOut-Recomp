// operator delete: forwards the pointer in a0 to Heap_Free (0x268500); v0 is the callee's.
// (Called by the deleting destructors as `if (flags & 1) 0x102610(this)`.) 558 call sites.
#include "theseus.h"

void OperatorDelete_00102610(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.call(0x268500, 0x102620);
    g.ret_void();  // v0 is the callee's
}

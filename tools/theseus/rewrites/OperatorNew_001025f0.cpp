// operator new: Heap_AllocAligned (0x268530) with alignment 4; v0 is the callee's. 308 call sites.
#include "theseus.h"

void OperatorNew_001025f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg(1, 4);  // addiu a1, $zero, 4 (jal delay slot: set before the call)
    g.call(0x268530, 0x102600);
    g.ret_void();
}

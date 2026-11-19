// Deleting-destructor tail: if bit 0 of a1 is set, operator delete(this) (a1 is passed on as
// a1 & 1). v0 is the callee's, or unchanged. 143 call sites.
#include "theseus.h"

void DeletingDtor_002cc028(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg64(1, g.arg64(1) & 1);  // andi a1, a1, 1
    if (g.arg64(1) != 0)
        g.call(0x102610, 0x2CC040);
    g.ret_void();
}

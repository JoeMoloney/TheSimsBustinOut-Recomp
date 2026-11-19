// Deleting-destructor tail (same code as DeletingDtor_002cc028, but a call's return address
// is its own, so copies of a calling function can't share a rewrite). 58 call sites.
#include "theseus.h"

void DeletingDtor_003450f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg64(1, g.arg64(1) & 1);  // andi a1, a1, 1
    if (g.arg64(1) != 0)
        g.call(0x102610, 0x345110);
    g.ret_void();
}

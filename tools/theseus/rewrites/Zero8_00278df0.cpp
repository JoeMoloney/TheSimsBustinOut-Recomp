// Clears the two words at a0 and returns a0 (all 64 bits). 18 call sites (5 identical copies).
// theseus: copies 0x28a868, 0x2ae8c0, 0x320568, 0x3607a8
#include "theseus.h"

void Zero8_00278df0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0);
    g.write32(g.arg(0) + 4, 0);
    g.ret_copy_of_arg(0);
}

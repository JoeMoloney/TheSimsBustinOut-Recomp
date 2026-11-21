// Stores a1 at a0 and returns a0 (all 64 bits). 9 call sites (2 identical copies).
// theseus: copies 0x365678
#include "theseus.h"

void Word_Set_0035e4a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), g.arg(1));
    g.ret_copy_of_arg(0);
}

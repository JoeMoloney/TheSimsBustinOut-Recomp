// Stores the low bytes of a2 at a0+1 and a1 at a0. 1 call site.
#include "theseus.h"

void Bytes2_Set_002cc2f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0) + 1, static_cast<uint8_t>(g.arg(2)));
    g.write8(g.arg(0), static_cast<uint8_t>(g.arg(1)));
    g.ret_void();
}

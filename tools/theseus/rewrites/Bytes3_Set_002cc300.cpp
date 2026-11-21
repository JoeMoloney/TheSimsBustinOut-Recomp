// Stores the low bytes of a1, a2, a3 at a0+0, +1, +2 (e.g. an RGB triple). 6 call sites.
#include "theseus.h"

void Bytes3_Set_002cc300(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write8(p + 2, static_cast<uint8_t>(g.arg(3)));
    g.write8(p, static_cast<uint8_t>(g.arg(1)));  // gate: equivalent write16: the next store overwrites p+1
    g.write8(p + 1, static_cast<uint8_t>(g.arg(2)));
    g.ret_void();
}

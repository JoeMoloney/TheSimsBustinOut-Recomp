// Stores the low byte of a1 at a0+2. 3 call sites. (The map joined another function's tail after the `jr $ra`.)
#include "theseus.h"

void Byte2_Set_002cc318(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0) + 2, static_cast<uint8_t>(g.arg(1)));
    g.ret_void();
}

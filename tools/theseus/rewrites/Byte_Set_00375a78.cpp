// Stores the low byte of a1 at a0 and returns a0. 3 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Byte_Set_00375a78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0), static_cast<uint8_t>(g.arg(1)));
    g.ret_copy_of_arg(0);
}

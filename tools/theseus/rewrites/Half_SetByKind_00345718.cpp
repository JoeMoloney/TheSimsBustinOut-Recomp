// Stores the halfword a1 at a0+8 when a2 is 1 or 2, else at a0+0x14. 4 call sites. (The map
// joined another function's tail after the `jr $ra`.)
#include "theseus.h"

void Half_SetByKind_00345718(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const bool one_or_two = g.arg(2) - 1 < 2;
    g.write16(g.arg(0) + (one_or_two ? 8 : 0x14), static_cast<uint16_t>(g.arg(1)));
    g.ret_void();
}

// Initialiser: +0 = a1, halfword +8 = a2, +4 = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object_InitPtrHalf_00342490(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write16(p + 8, static_cast<uint16_t>(g.arg(2)));
    g.write32(p + 4, 0);
    g.ret_copy_of_arg(0);
}

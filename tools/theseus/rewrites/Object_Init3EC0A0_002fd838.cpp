// Small constructor: halfword +4 = a1, +0 = 0x3EC0A0 (vtable). Returns a0. 4 call sites.
#include "theseus.h"

void Object_Init3EC0A0_002fd838(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write16(g.arg(0) + 4, static_cast<uint16_t>(g.arg(1)));
    g.write32(g.arg(0), 0x3EC0A0);
    g.ret_copy_of_arg(0);
}

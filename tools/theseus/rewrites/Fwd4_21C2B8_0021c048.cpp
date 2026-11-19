// Calls 0x21C2B8 with a0 = word +4 of a0; v0 is the callee's (44 call sites).
#include "theseus.h"

void Fwd4_21C2B8_0021c048(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg(0, g.read32(g.arg(0) + 4));
    g.call(0x21C2B8, 0x21C058);
    g.ret_void();
}

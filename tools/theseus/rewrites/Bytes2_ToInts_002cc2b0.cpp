// *a1 = signed byte a0[0], *a2 = signed byte a0[1] (a2 written after a0[1] is read). v0 = the first. 2 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Bytes2_ToInts_002cc2b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t first = g.sread8(g.arg(0));
    g.write32(g.arg(1), static_cast<uint32_t>(first));
    const int32_t second = g.sread8(g.arg(0) + 1);
    SET_GPR_S32(ctx, 3, second);  // v1, as the original leaves it; gate: equivalent
    g.write32(g.arg(2), static_cast<uint32_t>(second));
    g.ret(static_cast<uint32_t>(first));
}

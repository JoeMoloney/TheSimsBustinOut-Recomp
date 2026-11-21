// Stores the low byte of a1 at a0+0 and returns it sign-extended (a signed char setter).
// 7 call sites.
#include "theseus.h"

void Byte0_SetS8_002cc2d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.arg(1);
    g.write8(g.arg(0) + 0, static_cast<uint8_t>(value));
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(value))));
}

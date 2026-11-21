// Stores the low byte of a1 at a0+1 and returns it sign-extended (a signed char setter).
// 7 call sites.
#include "theseus.h"

void Byte1_SetS8_002cc2e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.arg(1);
    g.write8(g.arg(0) + 1, static_cast<uint8_t>(value));
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(value))));
}

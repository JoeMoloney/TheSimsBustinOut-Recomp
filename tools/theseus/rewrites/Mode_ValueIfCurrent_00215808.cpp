// If a0 equals the global at 0x3F92BC (the mode id also used by Mode_TryEnter), returns the
// global at 0x3F92B4, else 0. The compare is on all 64 bits of a0 (`xor` + `movn`). 20 call sites.
#include "theseus.h"

void Mode_ValueIfCurrent_00215808(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t current = static_cast<int32_t>(g.read32(0x3F92BC));
    const uint32_t value = g.read32(0x3F92B4);
    if (current == g.sarg64(0))
    {
        g.ret(value);
        return;
    }
    SET_GPR_VEC(ctx, 2, GPR_VEC(ctx, 0));  // `movn v0, $zero, ...` copies all 128 bits of $zero
    g.ret_void();
}

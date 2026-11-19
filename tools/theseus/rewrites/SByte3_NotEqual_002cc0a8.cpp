// True if the 3-byte structs at a0 and a1 differ (see SByte3_Equal; 7 call sites).
#include "theseus.h"

void SByte3_NotEqual_002cc0a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    for (uint32_t i = 0; i < 3; ++i)
        if (g.read8(b + i) != g.read8(a + i))
        {
            g.ret(1);
            return;
        }
    g.ret(0);
}

// True if the 3-byte structs at a0 and a1 are equal (see SByte3_Copy; 10 call sites).
#include "theseus.h"

void SByte3_Equal_002cc070(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    for (uint32_t i = 0; i < 3; ++i)
        if (g.read8(b + i) != g.read8(a + i))
        {
            g.ret(0);
            return;
        }
    g.ret(1);
}

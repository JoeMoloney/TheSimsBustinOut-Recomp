// Copies the 16 bytes at a0+0x406C to a0+0x407C (a saved copy), one at a time. Returns the last byte. 2 call sites.
#include "theseus.h"

void Bytes16_Copy406C_0011ee60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint8_t b = 0;  // gate: equivalent 1: the loop always runs and sets it
    for (uint32_t i = 0; i < 16; ++i)
    {
        b = g.read8(g.arg(0) + 0x406C + i);  // gate: equivalent read16: kept as a byte
        g.write8(g.arg(0) + 0x407C + i, b);
    }
    SET_GPR_S32(ctx, 3, 0);  // v1: the loop test, as the original leaves it; gate: equivalent
    g.ret(b);
}

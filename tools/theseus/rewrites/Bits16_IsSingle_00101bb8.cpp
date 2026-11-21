// 1 if exactly one of the low 16 bits of a1 is set, else 0 (a0 is not read). 1 call site.
#include "theseus.h"

void Bits16_IsSingle_00101bb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t bits = g.arg(1) & 0xFFFF;
    uint32_t seen = 0;
    for (uint32_t bit = 0; bit < 16; ++bit)  // gate: equivalent <=,17: bit 16 of a masked 16-bit value is 0
        if ((bits >> bit) & 1)
        {
            if (seen != 0)
            {
                g.ret(0);
                return;
            }
            seen = 1;
        }
    g.ret(seen);
}

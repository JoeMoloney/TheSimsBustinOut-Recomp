// The OR of the six u16 values at a0+0x14 + 8*i (i = 0..5). 1 call site.
#include "theseus.h"

void Slots8_OrU16At14_00101cd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t bits = 0;
    for (uint32_t i = 0; i < 6; ++i)
        bits |= g.read16(g.arg(0) + 8 * i + 0x14);
    g.ret(bits);
}

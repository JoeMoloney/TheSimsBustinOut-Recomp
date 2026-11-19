// 0 if the pointer at +0x130 of a0 is null, else whether word +0x54 of that object is nonzero
// (7 call sites).
#include "theseus.h"

void Field130_Field54NonZero_0020a6f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 0x130);
    if (obj == 0)
        g.ret(0);
    else
        g.ret(g.read32(obj + 0x54) != 0 ? 1 : 0);
}

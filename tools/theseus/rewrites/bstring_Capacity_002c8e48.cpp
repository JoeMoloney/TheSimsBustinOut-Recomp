// String capacity: the rep (+0 of the string) holds it at +8 (bstring.cpp layout).
// 6 call sites over 2 identical copies.
// theseus: copies 0x2cb268
#include "theseus.h"

void bstring_Capacity_002c8e48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.read32(g.arg(0));
    g.ret(g.read32(rep + 8));
}

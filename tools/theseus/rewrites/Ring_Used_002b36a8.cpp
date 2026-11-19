// Distance from position +0x2C to position +0x28 of a0 in a ring of size +0x30: (+0x28) -
// (+0x2C), plus the size if +0x28 is below +0x2C (signed). 3 call sites.
#include "theseus.h"

void Ring_Used_002b36a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint32_t from = g.read32(a + 0x2C);
    uint32_t to = g.read32(a + 0x28);
    if (static_cast<int32_t>(to) < static_cast<int32_t>(from))
        to += g.read32(a + 0x30);
    g.ret(to - from);
}

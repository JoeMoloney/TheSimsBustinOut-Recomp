// 0 if the pointer at +8 of a0 is null; otherwise whether bits 2..3 of word +0x5C of that
// object equal 1 (4 call sites).
#include "theseus.h"

void Field8_Mode5C_Is1_002de7d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 8);
    if (obj == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t flags = g.read32(obj + 0x5C);  // gate: equivalent read16: only bits 2-3 are used
    g.ret(((flags & 0xC) >> 2) == 1 ? 1 : 0);  // gate: equivalent 0xd: bit 0 is shifted out
}

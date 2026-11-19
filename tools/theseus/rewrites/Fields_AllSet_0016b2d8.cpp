// True if the words at +0x4C, +0x50, +0x58, +0x5C and +0x60 of a0 are all nonzero, tested in
// that order (4 call sites).
#include "theseus.h"

void Fields_AllSet_0016b2d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    g.ret(g.read32(a + 0x4C) && g.read32(a + 0x50) && g.read32(a + 0x58) && g.read32(a + 0x5C) && g.read32(a + 0x60) ? 1 : 0);
}

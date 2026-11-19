// True if the objects in a0 and a1 have the same word at +0x34 (10 call sites).
#include "theseus.h"

void SameField34_0031e4b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x34) == g.read32(g.arg(1) + 0x34) ? 1 : 0);
}

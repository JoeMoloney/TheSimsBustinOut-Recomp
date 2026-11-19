// Word +0x10 of the object that +0x328 of a0 points to (2 call sites).
#include "theseus.h"

void Field328_Field10_00362348(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.arg(0) + 0x328) + 0x10));
}

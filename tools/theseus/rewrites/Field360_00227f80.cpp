// Returns the word at +0x360 of the object in a0 (12 call sites).
#include "theseus.h"

void Field360_00227f80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x360));
}

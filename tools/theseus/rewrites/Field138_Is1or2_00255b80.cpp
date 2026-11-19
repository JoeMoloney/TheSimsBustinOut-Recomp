// True if the word at +0x138 of a0 is 1 or 2 (4 call sites).
#include "theseus.h"

void Field138_Is1or2_00255b80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x138) - 1 < 2 ? 1 : 0);
}

// True if the word at +0x134 of a0 is 1 or 2 (7 call sites).
#include "theseus.h"

void Field134_Is1or2_00255b90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x134) - 1 < 2 ? 1 : 0);
}

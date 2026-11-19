// True if the word at a0 is below the word at a1, unsigned (4 call sites).
#include "theseus.h"

void Field0_LessU_00365640(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0)) < g.read32(g.arg(1)) ? 1 : 0);
}

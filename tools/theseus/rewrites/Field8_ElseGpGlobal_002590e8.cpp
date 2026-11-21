// 1 if the word at a0+8 is 0, else the global at gp-0x7A24. 1 call site.
#include "theseus.h"

void Field8_ElseGpGlobal_002590e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 8) == 0 ? 1 : g.read32(g.gp() - 0x7A24));
}

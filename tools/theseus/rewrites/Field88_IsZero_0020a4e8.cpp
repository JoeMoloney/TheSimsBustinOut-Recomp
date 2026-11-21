// 1 if the word at a0+0x88 is 0, else 0. 1 call site.
#include "theseus.h"

void Field88_IsZero_0020a4e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x88) == 0 ? 1 : 0);
}

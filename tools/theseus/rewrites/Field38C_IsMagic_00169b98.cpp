// 1 if the word at a0+0x38C is 0x30AA9779, else 0. 1 call site.
#include "theseus.h"

void Field38C_IsMagic_00169b98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x38C) == 0x30AA9779 ? 1 : 0);
}

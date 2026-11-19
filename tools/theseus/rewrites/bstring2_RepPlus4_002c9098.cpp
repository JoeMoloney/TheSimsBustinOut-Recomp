// The rep pointer at +0 of the string plus 4 (bstring2.cpp; 4 call sites).
#include "theseus.h"

void bstring2_RepPlus4_002c9098(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0)) + 4);
}

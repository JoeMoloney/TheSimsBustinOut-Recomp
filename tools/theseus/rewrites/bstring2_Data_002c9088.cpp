// The string's character data: rep pointer (+0), then the rep's data pointer (+0)
// (bstring2.cpp; 9 call sites).
#include "theseus.h"

void bstring2_Data_002c9088(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.read32(g.arg(0));
    g.ret(g.read32(rep));
}

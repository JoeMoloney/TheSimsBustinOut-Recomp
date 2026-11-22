// When a0+0x88 is nonzero and a0+0xEC is 0, sets a0+0xEC to 1. Returns 0 when +0x88 is 0, else 1. 1 call site.
#include "theseus.h"

void FieldEC_SetOnceIf88_0020a4f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0) + 0x88) == 0)
    {
        g.ret(0);
        return;
    }
    if (g.read32(g.arg(0) + 0xEC) == 0)
        g.write32(g.arg(0) + 0xEC, 1);  // gate: equivalent write16: the field is 0 here, its upper half stays 0
    g.ret(1);
}

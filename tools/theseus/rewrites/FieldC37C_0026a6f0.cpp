// The word at a0+0xC37C (written as 0x8000 + 0x437C). 1 call site.
#include "theseus.h"

void FieldC37C_0026a6f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0xC37C));
}

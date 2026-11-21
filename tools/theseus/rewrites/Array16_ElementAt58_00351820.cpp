// Address of 16-byte element [a0+0x58] of the array *a0. 1 call site.
#include "theseus.h"

void Array16_ElementAt58_00351820(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0)) + (g.read32(g.arg(0) + 0x58) << 4));
}

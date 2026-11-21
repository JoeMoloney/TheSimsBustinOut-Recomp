// Address of 16-byte element a1 of the array *a0: [a0] + a1 * 16. 1 call site.
#include "theseus.h"

void Array16_ElementAddr_00351570(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0)) + (g.arg(1) << 4));
}

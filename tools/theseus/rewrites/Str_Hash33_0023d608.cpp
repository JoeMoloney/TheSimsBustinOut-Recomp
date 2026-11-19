// String hash h = h * 33 + c (djb2 shape) over the bytes at a0, starting from a1. The
// terminating 0 is folded in too (the add sits in the loop branch's delay slot). 3 call sites.
#include "theseus.h"

void Str_Hash33_0023d608(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), h = g.arg(1);
    uint32_t c;
    do
    {
        c = g.read8(p++);
        h = h * 33 + c;
    } while (c != 0);
    g.ret(h);
}

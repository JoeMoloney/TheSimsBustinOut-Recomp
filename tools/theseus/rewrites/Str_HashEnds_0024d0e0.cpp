// Quick string hash: a1 (length) + T[last byte] + T[first byte], T = the byte table at 0x3A7990
// (a0 = string, a1 = its length). 1 call site.
#include "theseus.h"

void Str_HashEnds_0024d0e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0), len = g.arg(1);
    const uint32_t last = g.read8(0x003A7990 + g.read8(s + len - 1));
    const uint32_t first = g.read8(0x003A7990 + g.read8(s));
    g.ret(len + last + first);
}

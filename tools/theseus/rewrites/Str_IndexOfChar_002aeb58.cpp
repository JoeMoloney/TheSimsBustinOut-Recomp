// Index of the first byte equal to the signed byte a1 in the string *a0, or -1. 1 call site.
#include "theseus.h"

void Str_IndexOfChar_002aeb58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int8_t wanted = static_cast<int8_t>(g.arg(1));
    uint32_t p = g.read32(g.arg(0));
    for (int32_t i = 0; g.sread8(p) != 0; ++i, ++p)  // gate: equivalent read8: only compared with 0
        if (g.sread8(p) == wanted)
        {
            g.ret(i);
            return;
        }
    g.ret(0xFFFFFFFF);
}

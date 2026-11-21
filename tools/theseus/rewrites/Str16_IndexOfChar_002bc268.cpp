// Index of the first 16-bit character equal to (a1 & 0xFFFF) in the string *a0, or -1. 1 call site.
#include "theseus.h"

void Str16_IndexOfChar_002bc268(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint16_t wanted = static_cast<uint16_t>(g.arg(1));
    uint32_t p = g.read32(g.arg(0));
    for (int32_t i = 0;; ++i, p += 2)
    {
        const uint16_t c = g.read16(p);  // gate: equivalent read32: truncated to uint16_t
        if (c == 0)
            break;
        if (c == wanted)
        {
            g.ret(i);
            return;
        }
    }
    g.ret(0xFFFFFFFF);
}

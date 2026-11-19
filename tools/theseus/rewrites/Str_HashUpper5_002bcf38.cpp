// Case-insensitive string hash of the bytes at a1: h = h * 5 + c with 'a'..'z' mapped to
// 'A'..'Z' and characters taken as signed bytes; returns h & (word +0x10 of a0). 3 call sites.
#include "theseus.h"

void Str_HashUpper5_002bcf38(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(1), h = 0;
    for (int32_t c; (c = g.sread8(p)) != 0; ++p)
    {
        if (static_cast<uint32_t>(c - 'a') < 26)
            c = static_cast<int8_t>(c - 0x20);
        h = h * 5 + static_cast<uint32_t>(c);
    }
    g.ret(h & g.read32(g.arg(0) + 0x10));
}

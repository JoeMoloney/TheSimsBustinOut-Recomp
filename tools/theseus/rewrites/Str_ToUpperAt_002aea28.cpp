// Upper-cases (a-z only) the string whose pointer is at *a0, in place. Returns a0. 2 call sites (the same code as Str_ToUpperAt_002bb098). (The map joined another function's epilogue after the `jr $ra`.)
#include "theseus.h"

void Str_ToUpperAt_002aea28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t p = g.read32(g.arg(0));; ++p)
    {
        const uint8_t c = g.read8(p);  // gate: equivalent read16: kept as a byte
        if (c == 0)
            break;
        if (static_cast<uint32_t>(c - 0x61) < 26)
            g.write8(p, static_cast<uint8_t>(c - 0x20));
    }
    g.ret_copy_of_arg(0);
}

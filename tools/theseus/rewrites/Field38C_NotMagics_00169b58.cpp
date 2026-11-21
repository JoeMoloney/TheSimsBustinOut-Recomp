// 1 if [a0+0x350] is 4; else 0 if [a0+0x38C] is 0x92D8AE4A or 0x6BCF6EE9, else 1. 1 call site.
#include "theseus.h"

void Field38C_NotMagics_00169b58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t o = g.arg(0);
    if (g.read32(o + 0x350) == 4)
    {
        g.ret(1);
        return;
    }
    const uint32_t magic = g.read32(o + 0x38C);
    g.ret(magic == 0x92D8AE4A || magic == 0x6BCF6EE9 ? 0 : 1);
}

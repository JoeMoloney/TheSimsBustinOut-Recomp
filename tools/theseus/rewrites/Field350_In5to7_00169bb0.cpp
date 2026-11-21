// 0 if [a0+0x35C] is non-zero or [a0+0x358] is zero; else 1 if [a0+0x350] is 5, 6 or 7
// (unsigned (x - 5) < 3). 1 call site.
#include "theseus.h"

void Field350_In5to7_00169bb0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t o = g.arg(0);
    if (g.read32(o + 0x35C) != 0 || g.read32(o + 0x358) == 0)
    {
        g.ret(0);
        return;
    }
    g.ret(g.read32(o + 0x350) - 5u < 3u ? 1 : 0);
}

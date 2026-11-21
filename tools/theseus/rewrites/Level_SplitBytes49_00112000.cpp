// Splits a level a1 into two bytes: 0 -> both 0; 1..5 -> +0x49 = 6 - a1, +0x4A = 0; 6..10 -> +0x49 = 0, +0x4A = a1 - 5; otherwise nothing. Returns -1 for 0, else the value computed last (6 - a1, or a1 - 5). 1 call site.
#include "theseus.h"

void Level_SplitBytes49_00112000(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0), level = g.arg(1);
    if (g.arg64(1) == 0)
    {
        g.write8(p + 0x4A, 0);
        g.write8(p + 0x49, 0);
        g.ret(0xFFFFFFFFu);
        return;
    }
    if (level - 1 < 5)
    {
        g.write8(p + 0x4A, 0);
        g.write8(p + 0x49, static_cast<uint8_t>(6 - level));
        g.ret(6 - level);
        return;
    }
    if (level - 6 < 5)
    {
        g.write8(p + 0x49, 0);
        g.write8(p + 0x4A, static_cast<uint8_t>(level - 5));
    }
    g.ret(level - 5);
}

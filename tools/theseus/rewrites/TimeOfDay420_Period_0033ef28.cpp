// Period of the day for the hour (s16) at a0+0x420: before 6 -> 2, 6 -> 3, 7..17 -> 0,
// 18 -> 1, after 18 -> 2. 3 call sites.
#include "theseus.h"

void TimeOfDay420_Period_0033ef28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int16_t hour = g.sread16(g.arg(0) + 0x420);  // gate: equivalent read16: stored in an int16_t
    if (hour < 6)
        g.ret(2);
    else if (hour == 6)
        g.ret(3);
    else if (hour < 18)
        g.ret(0);
    else
        g.ret(hour == 18 ? 1 : 2);
}

// h = [a0+0x24] + [a0+0x2BC]; returns h if h < 12, else h - 12, or 11 when the flag at a0+0x2C0
// is 0 (a 12-hour wrap). 2 call sites.
#include "theseus.h"

void HourWrap12_0035cee0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t object = g.arg(0);
    const int32_t h = static_cast<int32_t>(g.read32(object + 0x24) + g.read32(object + 0x2BC));
    if (h < 12)
    {
        g.ret(h);
        return;
    }
    g.ret(g.read32(object + 0x2C0) == 0 ? 11 : h - 12);
}

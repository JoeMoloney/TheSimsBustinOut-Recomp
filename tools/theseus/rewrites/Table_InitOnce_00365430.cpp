// One-time initialiser of a table object: if +0 (initialised flag) is already nonzero it is just set to 1. Else: +8 = a2, +0xC = a0 + 0x1C (inline word array), +0xA8 = a0 + 0x44, +4 = a1 (count), +0x18 = 0, the first a1 array words = 0x80000000 (empty marker), +0 = 1. Returns 1. 3 call sites.
#include "theseus.h"

void Table_InitOnce_00365430(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t t = g.arg(0);
    if (g.read32(t) == 0)
    {
        g.write32(t + 8, g.arg(2));
        g.write32(t + 0xC, t + 0x1C);
        g.write32(t + 0xA8, t + 0x44);
        g.write32(t + 4, g.arg(1));
        g.write32(t + 0x18, 0);
        if (g.sarg64(1) > 0)  // `blez` tests the whole register
        {
            int32_t i = 0;
            do
            {
                g.write32(g.read32(t + 0xC) + (static_cast<uint32_t>(i) << 2), 0x80000000u);
                ++i;
            } while (i < static_cast<int32_t>(g.read32(t + 4)));
        }
    }
    g.write32(t, 1);
    g.ret(1);
}

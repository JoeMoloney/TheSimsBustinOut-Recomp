// Removes the first occurrence of a0 (compared as a whole register) from the 10-word table at 0x431A08: the later entries move down one and the last becomes 0. Returns 1 if found, else 0. 2 call sites.
#include "theseus.h"

void Table431A08_Remove_002259b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t base = 0x431A08;
    const int64_t value = g.sarg64(0);
    int32_t i = 0;
    while (static_cast<int32_t>(g.read32(base + (static_cast<uint32_t>(i) << 2))) != value)
    {
        if (++i >= 10)
        {
            g.ret(0);
            return;
        }
    }
    for (; i < 9; ++i)
        g.write32(base + (static_cast<uint32_t>(i) << 2), g.read32(base + (static_cast<uint32_t>(i) << 2) + 4));
    g.write32(base + 0x24, 0);
    g.ret(1);
}

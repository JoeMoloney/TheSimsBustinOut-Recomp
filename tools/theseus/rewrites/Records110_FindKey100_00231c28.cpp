// The first of the [a0+0x4504] records of 0x110 bytes at a0+0x4508 whose key (+0x100)
// equals a1 (all 64 bits), or 0. 1 call site.
#include "theseus.h"

void Records110_FindKey100_00231c28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 0x4504));
    const int64_t key = g.sarg64(1);
    uint32_t record = g.arg(0) + 0x4508;
    for (int32_t i = 0; i < count; ++i, record += 0x110)
        if (static_cast<int32_t>(g.read32(record + 0x100)) == key)
        {
            g.ret(record);
            return;
        }
    g.ret(0);
}

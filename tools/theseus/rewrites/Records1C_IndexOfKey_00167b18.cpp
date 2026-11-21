// Index of the first of the [a0+0x16004] 28-byte records at a0+0x8004 whose first word equals a1
// (all 64 bits), or -1. 1 call site.
#include "theseus.h"

void Records1C_IndexOfKey_00167b18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t count = static_cast<int32_t>(g.read32(g.arg(0) + 0x16004));
    const int64_t key = g.sarg64(1);
    uint32_t record = g.arg(0) + 0x8004;
    for (int32_t i = 0; i < count; ++i, record += 0x1C)
        if (static_cast<int32_t>(g.read32(record)) == key)
        {
            g.ret(i);
            return;
        }
    g.ret(0xFFFFFFFF);
}

// The first of the [[a0]+0x16004] 28-byte records at [a0]+0x8004 whose s16 at +0xC equals the s16
// a1, or 0 (sibling of Records1C_IndexOfKey_00167b18). 1 call site.
#include "theseus.h"

void Records1C_FindKey16_00167988(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t holder = g.read32(g.arg(0));
    const int16_t key = static_cast<int16_t>(g.arg(1));
    const uint32_t records = holder + 0x8004;
    for (int32_t i = 0; i < static_cast<int32_t>(g.read32(holder + 0x16004)); ++i)
    {
        const uint32_t record = records + static_cast<uint32_t>(i) * 0x1C;
        if (g.sread16(record + 0xC) == key)
        {
            g.ret(record);
            return;
        }
    }
    g.ret(0);
}

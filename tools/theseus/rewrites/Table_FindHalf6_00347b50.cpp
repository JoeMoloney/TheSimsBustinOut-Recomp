// On the first call (global 0x3F9188 == 0) sets it and zeroes the default entry'\''s halfwords 0x3F9260..0x3F9266. Then searches the counted array of 8-byte entries [a0+4] (count in the word before it) for the first whose signed halfword +6 equals a1 (whole register). Returns that entry, or the default entry 0x3F9260. 1 call site.
#include "theseus.h"

void Table_FindHalf6_00347b50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x3F9188) == 0)
    {
        g.write16(0x3F9260, 0);
        g.write32(0x3F9188, 1);
        g.write16(0x3F9262, 0);
        g.write16(0x3F9264, 0);
        g.write16(0x3F9266, 0);
    }
    const uint32_t entries = g.read32(g.arg(0) + 4);
    const int64_t key = g.sarg64(1);
    for (int32_t i = 0; i < static_cast<int32_t>(entries == 0 ? 0 : g.read32(entries - 4)); ++i)
    {
        const uint32_t e = entries + (static_cast<uint32_t>(i) << 3);
        if (static_cast<int16_t>(g.read16(e + 6)) == key)
        {
            g.ret(e);
            return;
        }
    }
    g.ret(0x3F9260);
}

// Looks a1 up among the 10 keys of the 8-byte record table at 0x3A8D38 (first match): stores the record'\''s address at a0+0xC and the key at a0+4. Nothing when not found. v0 = 0x3B0000 when the first record matches, else 0x3A8D38. 2 call sites.
#include "theseus.h"

void Table3A8D38_FindKey_003694b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);  // compared as a whole register
    for (uint32_t i = 0; i < 10; ++i)
    {
        const uint32_t rec = 0x3A8D38 + (i << 3);
        if (static_cast<int32_t>(g.read32(rec)) == key)
        {
            g.write32(g.arg(0) + 0xC, rec);
            g.write32(g.arg(0) + 4, g.arg(1));
            g.ret(i == 0 ? 0x3B0000 : 0x3A8D38);
            return;
        }
    }
    g.ret(0x3A8D38);
}

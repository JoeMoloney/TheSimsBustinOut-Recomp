// Reads two words for the current object: from the global object [0x44A9F0] (none: nothing, returns 0) through +0x3A18 -> +0 -> +0x54 = rec; when rec'\''s +0 has low 15 bits 0x12 and bit 15 set: info = [[rec+0x4C]+8], *a0 = info+0x1C, *a1 = info+0x20 (each when the pointer is nonzero); otherwise *a0 = *a1 = 0 (when nonzero). v0 is what the original leaves in it. 2 call sites.
#include "theseus.h"

void Global44A9F0_GetPair1C20_002330e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t g0 = g.read32(0x44A9F0);
    if (g0 == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t obj = g.read32(g0 + 0x3A18);
    const uint32_t rec = g.read32(g.read32(obj) + 0x54);
    uint32_t v0 = obj;
    bool ok = false;
    if (rec != 0)
    {
        const uint32_t flags = g.read32(rec);
        v0 = flags & 0x7FFF;
        if (v0 == 0x12)
        {
            v0 = flags & 0x8000;
            ok = v0 != 0;
        }
    }
    if (!ok)
    {
        if (g.arg64(0) != 0)
            g.write32(g.arg(0), 0);
        if (g.arg64(1) != 0)
            g.write32(g.arg(1), 0);
        g.ret(v0);
        return;
    }
    const uint32_t info_holder = g.read32(rec + 0x4C);
    if (g.arg64(0) != 0)
    {
        v0 = g.read32(info_holder + 8);
        g.write32(g.arg(0), g.read32(v0 + 0x1C));
    }
    if (g.arg64(1) != 0)
    {
        v0 = g.read32(info_holder + 8);
        g.write32(g.arg(1), g.read32(v0 + 0x20));
    }
    g.ret(v0);
}

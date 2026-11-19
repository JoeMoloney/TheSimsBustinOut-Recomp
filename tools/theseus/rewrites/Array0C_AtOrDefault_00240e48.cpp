// Entry a1 of the word array at +0xC of a0 (count at +0x14, signed compares on all 64 bits of
// a1); the global default at 0x44B048 when a1 is out of range or the entry is 0 (3 call sites).
#include "theseus.h"

void Array0C_AtOrDefault_00240e48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    if (g.sarg64(1) >= 0 && g.sarg64(1) < static_cast<int32_t>(g.read32(a + 0x14)))
    {
        const uint32_t v = g.read32(g.read32(a + 0xC) + (g.arg(1) << 2));
        if (v != 0)
        {
            g.ret(v);
            return;
        }
    }
    g.ret(g.read32(0x44B048));
}

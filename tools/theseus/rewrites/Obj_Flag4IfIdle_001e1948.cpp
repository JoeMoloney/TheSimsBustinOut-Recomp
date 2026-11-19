// 0 for a null a0 (all 64 bits) or when word +0x60 is nonzero; otherwise whether the signed
// byte at +4 is nonzero (4 call sites).
#include "theseus.h"

void Obj_Flag4IfIdle_001e1948(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(0) == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t a = g.arg(0);
    if (g.read32(a + 0x60) != 0)
    {
        g.ret(0);
        return;
    }
    g.ret(g.sread8(a + 4) != 0 ? 1 : 0);  // gate: equivalent read8: only compared with 0
}

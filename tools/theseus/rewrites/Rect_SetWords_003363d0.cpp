// Stores a4 (t0) at a0+0xC, a1 at +0, a2 at +4, a3 at +8. 1 call site. (The map joined halfword setters after the `jr $ra`.)
#include "theseus.h"

void Rect_SetWords_003363d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, g.arg(4));
    g.write32(p, g.arg(1));
    g.write32(p + 4, g.arg(2));
    g.write32(p + 8, g.arg(3));
    g.ret_void();
}

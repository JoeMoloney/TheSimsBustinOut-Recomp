// Stores a1 at a0+0x38 when a1 < 8 (unsigned, whole register); returns that test. 1 call site.
#include "theseus.h"

void Field38_SetIfBelow8_00341d68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t ok = g.arg64(1) < 8;
    if (ok)
        g.write32(g.arg(0) + 0x38, g.arg(1));
    g.ret(ok);
}

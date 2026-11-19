// Entry a1 of the 2-word global table at 0x3F92C8 if a1 < 2 (signed, all 64 bits), else 0
// (14 call sites).
#include "theseus.h"

void GlobalPair_Entry_002581c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.sarg64(1) < 2)
        g.ret(g.read32(0x3F92C8 + (g.arg(1) << 2)));
    else
        g.ret(0);
}

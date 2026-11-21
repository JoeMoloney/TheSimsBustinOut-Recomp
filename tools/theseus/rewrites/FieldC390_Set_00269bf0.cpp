// Stores a1 at a0+0xC390. v0 = 0x8000 (the offset'\''s upper part). 2 call sites.
#include "theseus.h"

void FieldC390_Set_00269bf0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0xC390, g.arg(1));
    g.ret(0x8000);
}

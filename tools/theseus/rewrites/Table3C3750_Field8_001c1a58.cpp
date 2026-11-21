// Field +8 of the 12-byte entry [a0+0x104] of the table at 0x3C3750. 1 call site.
#include "theseus.h"

void Table3C3750_Field8_001c1a58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x003C3750 + g.read32(g.arg(0) + 0x104) * 12 + 8));
}

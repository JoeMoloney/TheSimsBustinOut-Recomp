// Stores 0x3A8AD8 (a vtable or static descriptor) at *a0; v0 = 0x3A8AD8. 4 call sites.
#include "theseus.h"

void VtablePtr3A8AD8_Set_002bb9c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x3A8AD8);
    g.ret(0x3A8AD8);
}

// Word a1 of the table at +0x10 of the object in a0 (17 call sites).
#include "theseus.h"

void Table10_Entry_002581b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + (g.arg(1) << 2) + 0x10));
}

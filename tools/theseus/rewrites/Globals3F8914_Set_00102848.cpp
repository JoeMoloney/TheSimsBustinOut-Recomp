// Sets the globals 0x3F8914 = a1, 0x3F7B1C ($gp - 0x7FD4) = 1, 0x3F8918 = a0, 0x3F891C = a2. Returns 1. 1 call site. (The map joined another function'\''s epilogue after the `jr $ra`.)
#include "theseus.h"

void Globals3F8914_Set_00102848(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F8914, g.arg(1));
    g.write32(0x3F7B1C, 1);
    g.write32(0x3F8918, g.arg(0));
    g.write32(0x3F891C, g.arg(2));
    g.ret(1);
}

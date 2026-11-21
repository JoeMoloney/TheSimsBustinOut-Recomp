// Initialiser: +0xC = 1, +0/+4/+8/+0x10/+0x14 = 0. Returns a0. 1 call site. (The map joined another function'\''s epilogue after the `jr $ra`.)
#include "theseus.h"

void Object18_InitC1_002bceb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 4, 0);
    g.write32(p + 0xC, 1);
    g.write32(p, 0);
    g.write32(p + 0x10, 0);
    g.write32(p + 0x14, 0);
    g.write32(p + 8, 0);
    g.ret_copy_of_arg(0);
}

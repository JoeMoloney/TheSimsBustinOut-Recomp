// Zeroes the four words at a0 (+8, +0xC, +0, +4). Returns a0. 3 call sites.
#include "theseus.h"

void Words4_Zero_00330ef8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 8, 0);
    g.write32(p + 0xC, 0);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.ret_copy_of_arg(0);
}

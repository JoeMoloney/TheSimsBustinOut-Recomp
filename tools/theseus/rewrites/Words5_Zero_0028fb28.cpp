// Zeroes the 5 words at a0. Returns a0. 2 call sites.
#include "theseus.h"

void Words5_Zero_0028fb28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t off = 0; off < 0x14; off += 4)  // gate: equivalent 0x15: off steps by 4
        g.write32(g.arg(0) + off, 0);
    g.ret_copy_of_arg(0);
}

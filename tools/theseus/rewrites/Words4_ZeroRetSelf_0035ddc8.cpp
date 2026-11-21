// Zeroes the 4 words at a0; returns a0. 1 call site.
#include "theseus.h"

void Words4_ZeroRetSelf_0035ddc8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t off = 0; off < 0x10; off += 4)  // gate: equivalent 0x11: off steps by 4
        g.write32(g.arg(0) + off, 0);
    g.ret_copy_of_arg(0);
}

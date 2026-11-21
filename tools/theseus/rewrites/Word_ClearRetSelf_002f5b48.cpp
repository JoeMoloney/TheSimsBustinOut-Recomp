// Zeroes the word at a0 and returns a0. 2 call sites.
#include "theseus.h"

void Word_ClearRetSelf_002f5b48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0);
    g.ret_copy_of_arg(0);
}

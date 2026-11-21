// Zeroes the word at a0+4 and returns a0. 2 call sites.
#include "theseus.h"

void Field4_Clear_00105b00(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.ret_copy_of_arg(0);
}

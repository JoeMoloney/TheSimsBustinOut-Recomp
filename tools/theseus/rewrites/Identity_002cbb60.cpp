// Returns a0 unchanged, all 64 bits (13 call sites).
#include "theseus.h"

void Identity_002cbb60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret_copy_of_arg(0);
}

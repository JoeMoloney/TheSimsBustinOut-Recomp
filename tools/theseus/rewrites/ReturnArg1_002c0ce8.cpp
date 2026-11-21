// Returns a1 (all 64 bits). 1 call site. (The map joined several setters after the `jr $ra`.)
#include "theseus.h"

void ReturnArg1_002c0ce8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret_copy_of_arg(1);
}

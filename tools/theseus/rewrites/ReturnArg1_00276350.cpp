// Returns a1 unchanged (`daddu v0, a1, $zero`: all 64 bits). 1 call site.
#include "theseus.h"

void ReturnArg1_00276350(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret_copy_of_arg(1);
}

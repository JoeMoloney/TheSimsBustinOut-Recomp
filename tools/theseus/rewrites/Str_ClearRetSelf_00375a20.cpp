// Terminates the string at a0 (first byte 0); returns a0. 1 call site.
#include "theseus.h"

void Str_ClearRetSelf_00375a20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0), 0);
    g.ret_copy_of_arg(0);
}

// Sets the word at a0 to -1; returns a0 (v1 = -1). 1 call site.
#include "theseus.h"

void Word_SetMinus1_002c0f28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0xFFFFFFFFu);
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

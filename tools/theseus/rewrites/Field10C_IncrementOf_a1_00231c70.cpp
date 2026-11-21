// Increments the word at a1+0x10C and returns a1 (a0 is unused). 2 call sites.
#include "theseus.h"

void Field10C_IncrementOf_a1_00231c70(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(1) + 0x10C;
    g.write32(field, g.read32(field) + 1);
    g.ret_copy_of_arg(1);
}

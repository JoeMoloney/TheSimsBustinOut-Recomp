// Adds a1 to the word at a0+4; returns a0 (v1 = the new value). 1 call site.
#include "theseus.h"

void Field4_AddRetSelf_003607f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 4) + g.arg(1);
    g.write32(g.arg(0) + 4, value);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(value));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

// Copies the word at a0+0x10 to a0+0x34 and sets the global 0x3F7E3C ($gp - 0x7CB4) to 1. Returns the copied word (v1 = 1). 1 call site.
#include "theseus.h"

void Field34_From10_SetFlag_00191468(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 0x10);
    g.write32(g.arg(0) + 0x34, value);
    g.write32(0x3F7E3C, 1);
    SET_GPR_S32(ctx, 3, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(value);
}

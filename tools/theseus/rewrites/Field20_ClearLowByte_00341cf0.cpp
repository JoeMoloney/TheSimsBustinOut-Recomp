// Clears the low byte of the word at a0+0x20; returns the new word (v1 = -0x100). 1 call site.
#include "theseus.h"

void Field20_ClearLowByte_00341cf0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 0x20) & 0xFFFFFF00u;
    g.write32(g.arg(0) + 0x20, value);  // gate: equivalent write16: only the low byte changes
    SET_GPR_S32(ctx, 3, -0x100);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(value);
}

// *a1 = the word at a0+0xB0, *a2 = 0x8800. Returns the word (v1 = 0x8800). 1 call site.
#include "theseus.h"

void FieldB0_GetWith8800_0028f718(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 0xB0);
    g.write32(g.arg(1), value);
    g.write32(g.arg(2), 0x8800);
    SET_GPR_S32(ctx, 3, 0x8800);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(value);
}

// Increments the word at a0; when it becomes 1, zeroes a0+0x1C. Returns the new count (v1 = 1). 1 call site.
#include "theseus.h"

void Counter_IncrementResetOnFirst_0035b040(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0)) + 1;
    g.write32(g.arg(0), value);
    if (value == 1)
        g.write32(g.arg(0) + 0x1C, 0);
    SET_GPR_S32(ctx, 3, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(value);
}

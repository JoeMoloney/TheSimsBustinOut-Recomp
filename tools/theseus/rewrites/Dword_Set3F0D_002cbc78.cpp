// Stores the 64-bit value 0x3F0D at a0; returns 1 (v1 = 0x3F0D). 1 call site.
#include "theseus.h"

void Dword_Set3F0D_002cbc78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write64(g.arg(0), 0x3F0D);
    SET_GPR_S32(ctx, 3, 0x3F0D);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(1);
}

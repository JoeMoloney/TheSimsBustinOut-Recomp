// A static initialiser (a1 == 0xFFFF and a0 != 0, whole registers): zeroes the 2 words at 0x455C48. v0 = 0xFFFF when a1 != 0xFFFF, else 0x450000 (v1 = 0x455C48 when it stores). 1 call site. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void StaticInit_455C48_002bfca0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF)
    {
        g.ret(0xFFFF);
        return;
    }
    if (g.arg64(0) != 0)
    {
        g.write32(0x455C48, 0);
        g.write32(0x455C4C, 0);
        SET_GPR_S32(ctx, 3, 0x455C48);  // scratch register for callers (not ABI-compared); gate: equivalent
    }
    g.ret(0x450000);
}

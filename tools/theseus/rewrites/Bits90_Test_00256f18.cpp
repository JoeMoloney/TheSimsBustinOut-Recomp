// Whether bit a1 (mod 32) of the word at +0x90 of a0 is set: 1, or 0 written as a copy of
// $zero (`movz v0, $zero`: all 128 bits). 4 call sites.
#include "theseus.h"

void Bits90_Test_00256f18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0) + 0x90) & (1u << (g.arg(1) & 31)))
    {
        g.ret(1);
        return;
    }
    SET_GPR_VEC(ctx, 2, GPR_VEC(ctx, 0));
    g.ret_void();
}

// Sets default layout values: +4 = 0x120, +0x44 = 1, +0xC = 0x170, +0x10 = 0x110, +8 = 0x130, +0 = 0. Returns 1 (v1/a1/a2/a3 = the constants, as the original leaves them). 1 call site.
#include "theseus.h"

void Layout_InitDefaults_0026c3b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 4, 0x120);
    g.write32(p + 0x44, 1);
    g.write32(p + 0xC, 0x170);
    g.write32(p + 0x10, 0x110);
    g.write32(p + 8, 0x130);
    g.write32(p, 0);
    SET_GPR_S32(ctx, 3, 0x120);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 0x170);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 0x110);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, 0x130);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(1);
}

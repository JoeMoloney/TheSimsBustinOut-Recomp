// A static initialiser (a1 == 0xFFFF and a0 != 0, whole registers): zeroes the words at 0x44AA08 + 0, +0x590, +0x404, +0x488, +0x50C. Returns 0x44AA08 when it stores, else 0xFFFF. 1 call site. (The map joined the caller-side wrapper after the `jr $ra`.)
#include "theseus.h"

void StaticInit_44AA08_00239d90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF || g.arg64(0) == 0)
    {
        if (g.arg64(1) == 0xFFFF)  // only decides the scratch register v1 below; gate: equivalent
            SET_GPR_S32(ctx, 3, 0x450000);  // scratch register for callers (not ABI-compared); gate: equivalent
        g.ret(0xFFFF);
        return;
    }
    const uint32_t b = 0x44AA08;
    g.write32(b, 0);
    g.write32(b + 0x590, 0);
    g.write32(b + 0x404, 0);
    g.write32(b + 0x488, 0);
    g.write32(b + 0x50C, 0);
    SET_GPR_S32(ctx, 3, 0x450000);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(b);
}

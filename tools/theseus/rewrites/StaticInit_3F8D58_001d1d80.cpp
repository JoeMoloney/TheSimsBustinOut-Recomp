// A static initialiser (a1 == 0xFFFF and a0 != 0, whole registers): 0x3F8D58 ($gp - 0x6D98) = 0x3C68A8, 0x3F8D5C = the global 0x3F7FF0 ($gp - 0x7B00). v0 = 0xFFFF when a1 != 0xFFFF, 0x3C0000 when a0 == 0, else 0x3C68A8 (v1 = the copied word). 1 call site.
#include "theseus.h"

void StaticInit_3F8D58_001d1d80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF)
    {
        g.ret(0xFFFF);
        return;
    }
    if (g.arg64(0) == 0)
    {
        g.ret(0x3C0000);
        return;
    }
    const uint32_t value = g.read32(0x3F7FF0);
    g.write32(0x3F8D58, 0x3C68A8);
    g.write32(0x3F8D5C, value);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(value));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x3C68A8);
}

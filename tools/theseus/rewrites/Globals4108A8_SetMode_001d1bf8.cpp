// Sets three fields of the global block 0x4108A8: a0 != 0 -> +0x44 = 1, +0x30 = 2, +0x28 = 1; a0 == 0 -> +0x30 = 1, +0x28 = 0. Returns 0x4108A8. 2 call sites. (The map joined other functions' epilogues after the `jr $ra`.)
#include "theseus.h"

void Globals4108A8_SetMode_001d1bf8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t b = 0x4108A8;
    if (g.arg64(0) == 0)
    {
        g.write32(b + 0x30, 1);
        g.write32(b + 0x28, 0);
    }
    else
    {
        g.write32(b + 0x44, 1);
        g.write32(b + 0x30, 2);
        g.write32(b + 0x28, 1);
    }
    g.ret(b);
}

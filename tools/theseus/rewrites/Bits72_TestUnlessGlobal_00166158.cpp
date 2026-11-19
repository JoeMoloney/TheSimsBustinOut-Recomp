// 0 if the global at 0x3F9078 is set; otherwise bit (a1 & 0xFFFF) (mod 32) of the 16-bit
// field +0x72 of the object in a0 (14 call sites).
#include "theseus.h"

void Bits72_TestUnlessGlobal_00166158(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x3F9078) != 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t bits = g.read16(g.arg(0) + 0x72);
    g.ret((bits >> ((g.arg(1) & 0xFFFF) & 31)) & 1);
}

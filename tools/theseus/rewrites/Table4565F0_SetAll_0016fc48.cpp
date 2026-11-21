// When the global 0x3FA65C is nonzero, sets the first word of each of the 16 twelve-byte records at 0x4565F0 to 1 (last record first). v0 = 0x450000 when the global is 0, else 0x4565E4 (what the loop leaves). 1 call site.
#include "theseus.h"

void Table4565F0_SetAll_0016fc48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x3FA65C) == 0)
    {
        g.ret(0x450000);
        return;
    }
    for (int32_t i = 15; i >= 0; --i)
        g.write32(0x4565F0 + 0xC * static_cast<uint32_t>(i), 1);
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x4565E4);
}

// Resets the 16 twelve-byte records at 0x4565F0: +0 = 1 for records 0, 14 and 15, else 0; +4 = 15; +8 = 0. Returns 0 (the last loop test). 1 call site.
#include "theseus.h"

void Table4565F0_Reset_00315858(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t i = 0; i < 16; ++i)
    {
        const uint32_t rec = 0x4565F0 + 0xC * i;
        g.write32(rec, (i == 0 || i - 14 < 3) ? 1 : 0);  // gate: equivalent 4,<=: i is at most 15, so only 14 and 15 pass either way
        g.write32(rec + 4, 15);
        g.write32(rec + 8, 0);
    }
    SET_GPR_S32(ctx, 3, 0x4565F0 + 0xC * 16);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, 16);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0);
}

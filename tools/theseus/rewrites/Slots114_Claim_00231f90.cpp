// Claims the first free entry (first word 0) of the 64 entries of 0x114 bytes at a0: sets its first word to 1 and increments the count a0+0x4500. Returns the entry, or 0 when all are used. 1 call site.
#include "theseus.h"

void Slots114_Claim_00231f90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t base = g.arg(0);
    const int32_t end = static_cast<int32_t>(base + 0x4500);
    for (uint32_t e = base; static_cast<int32_t>(e) < end; e += 0x114)
    {
        if (g.read32(e) == 0)
        {
            g.write32(e, 1);
            g.write32(base + 0x4500, g.read32(base + 0x4500) + 1);
            g.ret(e);
            return;
        }
    }
    g.ret(0);
}

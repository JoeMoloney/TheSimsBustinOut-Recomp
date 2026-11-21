// Appends the pair (a0, a1) to the 8-byte record table at 0x469348, indexed by the counter at
// 0x3A8A80 (incremented first; no bound check). v0 = 0x469348. 5 call sites.
#include "theseus.h"

void PairLog469348_Append_002af798(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t index = g.read32(0x3A8A80);
    const uint32_t entry = 0x469348 + (index << 3);
    g.write32(0x3A8A80, index + 1);
    g.write32(entry + 4, g.arg(1));
    g.write32(entry, g.arg(0));
    g.ret(0x469348);
}

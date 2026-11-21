// The largest u64 at +0x70 over the list from the global gp-0x785C (next at +0x7C), or 0;
// returned in all 64 bits of v0. 1 call site.
#include "theseus.h"

void List7C_MaxU64At70_002a2ff8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint64_t best = 0;
    for (uint32_t node = g.read32(g.gp() - 0x785C); node != 0; node = g.read32(node + 0x7C))
    {
        const uint64_t value = g.read64(node + 0x70);
        if (best < value)  // gate: equivalent <=: an equal value leaves the maximum the same
            best = value;
    }
    g.ret64(best);
}

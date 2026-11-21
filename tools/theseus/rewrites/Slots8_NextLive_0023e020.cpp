// Open-addressing table (see Slots8_FirstLive_0023dfd0): 8 << [a0] bytes of 8-byte slots at
// [a0+4]. Returns the first slot after the slot a1 whose key (+0) is neither 0 (empty) nor the
// deleted marker in the global 0x44B038, or 0 at the end of the table (unsigned compare).
// 2 call sites.
#include "theseus.h"

void Slots8_NextLive_0023e020(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 4) + (8u << (g.read32(g.arg(0)) & 31));  // gate: equivalent read16: only the low 5 bits are used
    const uint32_t deleted = g.read32(0x0044B038);
    for (uint32_t slot = g.arg(1) + 8; slot < end; slot += 8)
    {
        const uint32_t key = g.read32(slot);
        if (key != 0 && key != deleted)
        {
            g.ret(slot);
            return;
        }
    }
    g.ret(0);
}

// Open-addressing table: 1 << [a0] slots of 8 bytes at [a0+4]; returns the first slot whose key
// (+0) is neither 0 (empty) nor the deleted marker in the global 0x44B038, or 0. A slot count
// that is not positive (shift 31) gives 0. 2 call sites.
#include "theseus.h"

void Slots8_FirstLive_0023dfd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t count = static_cast<int32_t>(1u << (g.read32(g.arg(0)) & 31));  // gate: equivalent read16: only the low 5 bits are used
    if (count > 0)  // gate: equivalent >=: 1 << n is never 0
    {
        uint32_t slot = g.read32(g.arg(0) + 4);
        const uint32_t deleted = g.read32(0x0044B038);
        for (int32_t i = 0; i < count; ++i, slot += 8)
        {
            const uint32_t key = g.read32(slot);
            if (key != 0 && key != deleted)
            {
                g.ret(slot);
                return;
            }
        }
    }
    g.ret(0);
}

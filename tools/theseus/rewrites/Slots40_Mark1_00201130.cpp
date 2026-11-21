// If bit (8 + a1) of [a0+4] is set, ORs 0x10000000 | 0x800000 into the flags (+4) of the object
// in slot a1 of the pointer array at a0+0x40. Returns v0 as the original leaves it: the new flags,
// or the tested mask (0x100 << a1) when the bit is clear. 15 call sites.
#include "theseus.h"

void Slots40_Mark1_00201130(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t mask = 0x100u << (g.arg(1) & 31);
    if ((g.read32(g.arg(0) + 4) & mask) == 0)
    {
        g.ret(mask);
        return;
    }
    const uint32_t object = g.read32(g.arg(0) + (g.arg(1) << 2) + 0x40);
    const uint32_t flags = g.read32(object + 4) | 0x10000000 | 0x800000;
    g.write32(object + 4, flags);
    g.ret(flags);
}

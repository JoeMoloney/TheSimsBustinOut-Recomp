// Clears five vectors of the object at a0 (nothing to destroy: end = begin): +0xC (0x14-byte elements) and +0x4C (0x2C-byte elements), which the original walks with empty loops, then +0x1C, +0x2C, +0x3C. Returns the last begin. 2 call sites.
#include "theseus.h"

void Object_ClearVectors_00371330(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x10, g.read32(p + 0xC));
    g.write32(p + 0x50, g.read32(p + 0x4C));
    g.write32(p + 0x20, g.read32(p + 0x1C));
    g.write32(p + 0x30, g.read32(p + 0x2C));
    const uint32_t last = g.read32(p + 0x3C);
    g.write32(p + 0x40, last);
    g.ret(last);
}

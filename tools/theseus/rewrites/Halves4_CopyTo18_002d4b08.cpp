// Copies four halfwords from a1 to a0+0x18..0x1E (e.g. a rectangle), interleaved like the
// original. v0 = the halfword at a1+4. 5 call sites.
#include "theseus.h"

void Halves4_CopyTo18_002d4b08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0) + 0x18, src = g.arg(1);
    g.write16(dst, g.read16(src));  // gate: equivalent read32: kept as a halfword
    g.write16(dst + 2, g.read16(src + 2));  // gate: equivalent read32: kept as a halfword
    const uint16_t v0 = g.read16(src + 4);  // gate: equivalent read32: kept as a halfword
    g.write16(dst + 4, v0);
    g.write16(dst + 6, g.read16(src + 6));  // gate: equivalent read32: kept as a halfword
    g.ret(v0);
}

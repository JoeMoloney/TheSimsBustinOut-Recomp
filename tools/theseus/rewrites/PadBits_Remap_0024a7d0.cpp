// Moves the low byte's bits to new positions: bit 3 -> 0x800, 2 -> 0x400, 6 -> 0x1000, 0 -> 0x2000,
// 1 -> 0x4000, 5 -> 0x8000, 4 -> 0x10000 (bit 7 is dropped). 1 call site.
#include "theseus.h"

void PadBits_Remap_0024a7d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t in = g.arg(0);
    uint32_t out = (in << 8) & 0x800;  // gate: equivalent 0x801: bit 0 of in << 8 is 0
    if (in & 0x04) out |= 0x400;
    if (in & 0x40) out |= 0x1000;
    if (in & 0x01) out |= 0x2000;
    if (in & 0x02) out |= 0x4000;
    if (in & 0x20) out |= 0x8000;
    if (in & 0x10) out |= 0x10000;
    g.ret(out);
}

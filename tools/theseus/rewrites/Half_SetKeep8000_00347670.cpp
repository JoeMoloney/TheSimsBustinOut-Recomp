// Stores the 16-bit value a1 at a0, keeping the old bit 15 unless a1 is -1 (0xFFFF), which is
// stored as is. Returns the stored value (sign-extended a1, or-ed with the kept bit). 4 call
// sites.
#include "theseus.h"

void Half_SetKeep8000_00347670(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const uint32_t value = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(g.arg(1))));
    if (value == 0xFFFFFFFFu)  // gate: equivalent 0x100000000: the general path stores and returns the same for -1
    {
        g.write16(p, 0xFFFF);
        g.ret(value);
        return;
    }
    const uint32_t merged = value | (g.read16(p) & 0x8000u);  // gate: equivalent read32: only bit 15 is kept
    g.write16(p, static_cast<uint16_t>(merged));
    g.ret(merged);
}

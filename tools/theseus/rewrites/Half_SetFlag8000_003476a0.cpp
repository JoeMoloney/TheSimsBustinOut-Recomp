// Sets (a1 != 0) or clears (a1 == 0) bit 15 of the halfword at a0, unless the halfword is 0xFFFF (left alone). Returns the new halfword, or -1. 1 call site.
#include "theseus.h"

void Half_SetFlag8000_003476a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const int16_t h = static_cast<int16_t>(g.read16(p));  // gate: equivalent read32: cast to int16_t
    if (h == -1)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    uint32_t value;
    if (g.arg64(1) == 0)
        value = g.read16(p) & 0x7FFFu;  // gate: equivalent read32: only bits 0-14 are kept
    else
        value = g.read16(p) | 0x8000u;
    g.write16(p, static_cast<uint16_t>(value));
    g.ret(value);
}

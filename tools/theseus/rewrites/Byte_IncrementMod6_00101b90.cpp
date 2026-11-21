// Increments the byte at a1, wrapping to 0 from 6 (stored before the wrap test, as in the original); returns the stored byte (v1 = whether it was below 6). 1 call site.
#include "theseus.h"

void Byte_IncrementMod6_00101b90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint8_t b = static_cast<uint8_t>(g.read8(g.arg(1)) + 1);  // gate: equivalent read16: kept as a byte
    g.write8(g.arg(1), b);
    const uint32_t below = b < 6;
    if (!below)
        g.write8(g.arg(1), 0);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(below));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(g.read8(g.arg(1)));
}

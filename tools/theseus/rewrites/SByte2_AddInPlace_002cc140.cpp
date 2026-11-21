// Adds the two bytes at a1 to the two bytes at a0 (wrapping; loads and stores in the original's
// order, so overlapping buffers behave the same); returns a0 (all 64 bits). 13 call sites.
#include "theseus.h"

void SByte2_AddInPlace_002cc140(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0), src = g.arg(1);
    const uint8_t add0 = g.read8(src);  // gate: equivalent read16: truncated to uint8_t
    const uint8_t b0 = g.read8(dst), b1 = g.read8(dst + 1);  // gate: equivalent read16: truncated to uint8_t
    g.write8(dst, static_cast<uint8_t>(b0 + add0));
    const uint8_t add1 = g.read8(src + 1);  // gate: equivalent read16: truncated to uint8_t
    g.write8(dst + 1, static_cast<uint8_t>(b1 + add1));
    g.ret_copy_of_arg(0);
}

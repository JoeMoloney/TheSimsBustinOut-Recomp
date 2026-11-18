// Copy a 3-byte struct byte by byte (30 callers). Replaces FUN_002cc050 (dst = a0, src = a1),
// returns dst. FUN_002cbf58 and FUN_002cc050 are identical copies.
#include "theseus.h"

void SByte3_Copy_002cc050(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0), src = g.arg(1);
    for (uint32_t i = 0; i < 3; ++i)  // read-then-write per byte, as the original (overlap-safe order)
        g.write8(dst + i, g.read8(src + i));  // gate: equivalent read16: write8 keeps only the low byte
    g.ret_copy_of_arg(0);
}

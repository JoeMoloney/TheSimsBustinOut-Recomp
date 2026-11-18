// Fill a 3-byte struct from two 28.4 fixed-point ints and a byte (24 callers). Replaces
// FUN_002cbf78 (self = a0, src = a1, c = a2), returns self:
//   self[0] = src[1] >> 4, self[1] = src[0] >> 4 (arithmetic shifts), self[2] = c.
// Memory accesses keep the original's order.
#include "theseus.h"

void SByte3_FromFixed4_002cbf78(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0), src = g.arg(1);
    g.write8(self + 0, static_cast<uint8_t>(static_cast<int32_t>(g.read32(src + 4)) >> 4));  // gate: equivalent read16: only bits 4..11 reach the byte
    const int32_t first = static_cast<int32_t>(g.read32(src + 0));  // gate: equivalent read16: only bits 4..11 reach the byte
    g.write8(self + 2, static_cast<uint8_t>(g.arg(2)));
    g.write8(self + 1, static_cast<uint8_t>(first >> 4));
    g.ret_copy_of_arg(0);
}

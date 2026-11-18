// Set a 3-byte struct {s8 a, s8 b, u8 c}; a and b become 0 if the value doesn't fit in a
// signed byte (47 callers). Replaces FUN_002cbfa8 (self = a0, values a1..a3), returns self.
// The range checks compare whole 64-bit registers; the check of b re-reads the stored byte.
#include "theseus.h"

void SByte3_SetClamped_002cbfa8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    const int64_t a = g.sarg64(1), b = g.sarg64(2);
    g.write8(self + 0, static_cast<uint8_t>(a));  // gate: equivalent write16: the extra byte (+1) is overwritten by the next store
    g.write8(self + 1, static_cast<uint8_t>(b));  // gate: equivalent write16: the extra byte (+2) is overwritten by the last store
    if (static_cast<int8_t>(a) != a)
        g.write8(self + 0, 0);
    if (g.sread8(self + 1) != b)
        g.write8(self + 1, 0);  // gate: equivalent write16: the extra byte (+2) is overwritten by the last store
    g.write8(self + 2, static_cast<uint8_t>(g.arg(3)));
    g.ret_copy_of_arg(0);
}

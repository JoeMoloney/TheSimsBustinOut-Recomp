// True if the 3-byte struct at a0 is less than the one at a1, comparing the signed bytes in
// order (see SByte3_Equal; 5 call sites).
#include "theseus.h"

void SByte3_Less_002cc0e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    for (uint32_t i = 0; i < 3; ++i)
    {
        const int32_t x = g.sread8(a + i), y = g.sread8(b + i);
        if (x < y)
        {
            g.set_arg64(3, 1);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(1);
            return;
        }
        if (x != y)
        {
            g.set_arg64(3, 0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(0);
            return;
        }
    }
    g.set_arg64(3, 0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
    g.ret(0);
}

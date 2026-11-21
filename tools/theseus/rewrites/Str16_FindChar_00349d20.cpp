// wcschr for 16-bit strings: pointer to the first character equal to (a1 & 0xFFFF) before the
// terminator, else 0 (searching for 0 gives 0). The result is the original's a0 register as
// `movn v0, a0` leaves it: unchanged (all 128 bits) on a first-character match, otherwise the
// advanced pointer sign-extended, with a0's upper 64 bits. 2 call sites.
#include "theseus.h"

void Str16_FindChar_00349d20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a0_entry = GPR_VEC(ctx, 4);  // gate: equivalent 5: only v0's upper half (not part of the ABI compare)
    const uint16_t wanted = static_cast<uint16_t>(g.arg(1));
    uint32_t p = g.arg(0);
    for (;;)
    {
        const uint16_t c = g.read16(p);  // gate: equivalent read32: truncated to uint16_t
        if (c == 0)
        {
            g.ret(0);
            return;
        }
        if (c == wanted)
            break;
        p += 2;
    }
    const uint64_t low = p == g.arg(0) ? g.arg64(0) : static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(p)));  // gate: equivalent 1: arg(1) only changes bits 32..63 of v0, not part of the ABI compare
    g.ret_with_upper(low, a0_entry);
}

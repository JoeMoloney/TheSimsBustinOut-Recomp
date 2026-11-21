// tolower for 'A'..'Z' (a0 + 0x20); any other value is returned as the whole a0 register
// (`movn v0, a0` copies all 128 bits). 1 call site.
#include "theseus.h"

void Char_ToLower_00268c28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a0_entry = GPR_VEC(ctx, 4);  // gate: equivalent 5: only v0's upper half (not part of the ABI compare)
    if (g.arg(0) - 0x41u > 25u)
        g.ret_with_upper(g.arg64(0), a0_entry);
    else
        g.ret(g.arg(0) + 0x20);
}

// Round a1 up to a multiple of 4 (on 64 bits after a 32-bit add), at least 8 (unsigned). A
// rounded size above 7 comes through `movn v0, a1`, so v0 keeps a1's upper 64 bits (4 call sites).
#include "theseus.h"

void AlignSize4_Min8_002b0930(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i entry_a1 = GPR_VEC(ctx, 5);  // only v0's upper half (not ABI-compared); gate: equivalent
    const uint64_t size = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.arg(1) + 3))) & ~uint64_t(3);
    if (size > 7)  // size is a multiple of 4; at 8 both paths give 8 and only v0's upper half differs; gate: equivalent
        g.ret_with_upper(size, entry_a1);
    else
        g.ret(8);
}

// 0xDCC550C4 if the global at 0x3B20F0 is 1, else 0xB5968ED8. The first comes through
// `movz v0, a0`, which gives v0 a0's upper 64 bits (4 call sites).
#include "theseus.h"

void Global3b20f0_PickConst_0020c2f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i entry_a0 = GPR_VEC(ctx, 4);  // only v0's upper half (not ABI-compared); gate: equivalent
    if (g.read32(0x3B20F0) == 1)
        g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(0xDCC550C4u))), entry_a0);
    else
        g.ret(0xB5968ED8u);
}

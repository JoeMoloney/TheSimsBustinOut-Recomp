// Decision table on the mode m = [[0x3F80B8]+0x14C] and a0 (all compares on 64 bits):
//   (m, a0) in {(2, 0), (1, 2), (0, 1)}: 1 if a1 == 0 and a2 == 1, else 0 (v0 cleared to all 128
//                                         bits by `movn v0, $zero`)
//   (m, a0) in {(2, 2), (1, 0), (0, 0)}: 1 if a1 == 0 and a2 == 2, else 0
//   otherwise 0. 2 call sites.
#include "theseus.h"

void GameMode14C_Allows_001648f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t m = static_cast<int32_t>(g.read32(g.read32(0x003F80B8) + 0x14C));
    const int64_t a0 = g.sarg64(0);
    const bool a1_zero = g.arg64(1) == 0;
    if ((m == 2 && a0 == 0) || (m == 1 && a0 == 2) || (m == 0 && a0 == 1))
    {
        if (a1_zero && g.arg64(2) == 1)
            g.ret(1);
        else
            g.ret_with_upper(0, _mm_setzero_si128());
        return;
    }
    if ((m == 2 && a0 == 2) || (m == 1 && a0 == 0) || (m == 0 && a0 == 0))
    {
        g.ret(a1_zero && g.arg64(2) == 2 ? 1 : 0);
        return;
    }
    g.ret(0);
}

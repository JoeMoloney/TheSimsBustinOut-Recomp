// Compares a1 with a2 (signed, whole registers) and stores the three results: +0x1D0 = a1 < a2, +0x1C8 = a1 == a2, +0x1CC = a1 > a2. Returns the equality flag. 2 call sites.
#include "theseus.h"

void Compare_StoreFlags1C8_003641c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t x = g.sarg64(1), y = g.sarg64(2);
    const uint32_t less = x < y, equal = x == y, greater = y < x;
    g.write32(g.arg(0) + 0x1D0, less);
    g.write32(g.arg(0) + 0x1C8, equal);
    g.write32(g.arg(0) + 0x1CC, greater);
    g.ret(equal);
}

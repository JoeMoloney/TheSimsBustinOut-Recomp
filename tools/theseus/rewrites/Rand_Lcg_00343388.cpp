// rand(): seed = seed * 0x41C64E6D + 0x3039 (the global 0x3F9184), returning the new seed >> 16 computed
// as the original does from 16-bit halves of the old seed (its high bits differ from a plain shift:
// the partial products are added without carries beyond 32 bits). 12 call sites.
#include "theseus.h"

void Rand_Lcg_00343388(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t seed = g.read32(0x003F9184);
    const uint32_t lo = seed & 0xFFFF, hi = seed >> 16;
    uint32_t result = ((lo * 0x4E6Du + 0x3039u) >> 16) + lo * 0x41C6u + hi * 0x4E6Du;
    result += (hi * 0x41C6u) << 16;
    g.write32(0x003F9184, seed * 0x41C64E6Du + 0x3039u);
    g.ret(result);
}

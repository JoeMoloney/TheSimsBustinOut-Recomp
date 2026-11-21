// Hash of the a1 bytes at a0: each byte rotated left by a shift that starts at 0 and steps by 3
// (mod 32), XORed together. The rotate is written as the original: c << s, plus c >> (32 - s)
// only when s >= 25 (a byte shifted by less than 25 cannot wrap). 1 call site.
#include "theseus.h"

void Bytes_XorRotHash_00141368(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t length = g.sarg64(1);
    uint32_t hash = 0, shift = 0;
    for (int64_t i = 0; i < length; ++i)
    {
        const uint32_t c = g.read8(g.arg(0) + static_cast<uint32_t>(i));
        uint32_t rotated = c << shift;
        if (shift >= 25)
            rotated ^= c >> ((32 - shift) & 31);
        hash ^= rotated;
        shift = (shift + 3) & 31;
    }
    g.ret(hash);
}

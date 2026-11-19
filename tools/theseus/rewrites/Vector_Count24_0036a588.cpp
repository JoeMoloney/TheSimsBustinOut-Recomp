// Number of 24-byte elements between +0 (begin) and +4 (end) of a0: (end - begin) times the
// inverse 0xAAAAAAAB, >> 3 signed (exact for multiples of 24). 4 call sites.
#include "theseus.h"

void Vector_Count24_0036a588(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    g.ret(static_cast<uint32_t>(static_cast<int32_t>((g.read32(a + 4) - g.read32(a)) * 0xAAAAAAABu) >> 3));
}

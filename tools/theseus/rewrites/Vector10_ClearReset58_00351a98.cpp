// Clears the vector at a0 (16-byte elements, nothing to destroy: end = begin; the original walks it with an empty loop) and sets +0x5C and +0x58 to -1. Returns -1. 2 call sites.
#include "theseus.h"

void Vector10_ClearReset58_00351a98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t v = g.arg(0);
    g.write32(v + 4, g.read32(v));
    g.write32(v + 0x5C, 0xFFFFFFFFu);
    g.write32(v + 0x58, 0xFFFFFFFFu);
    g.ret(0xFFFFFFFFu);
}

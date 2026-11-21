// The word at +8 of element a0 of the pointer array that the global 0x3FA5D4 holds (via its +0). 1 call site.
#include "theseus.h"

void GlobalArray3FA5D4_Field8_0015eb88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t array = g.read32(g.read32(0x003FA5D4));
    g.ret(g.read32(g.read32(array + (g.arg(0) << 2)) + 8));
}

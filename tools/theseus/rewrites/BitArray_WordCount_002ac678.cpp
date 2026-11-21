// The number of 32-bit words for the bit count at a0+4: (count + 31) >> 5 (unsigned). 1 call site.
#include "theseus.h"

void BitArray_WordCount_002ac678(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((g.read32(g.arg(0) + 4) + 31) >> 5);
}

// 0 if the bit array at a0 has no data ([a0] == 0); else its size in bytes: the bit count [a0+4]
// rounded up to whole 32-bit words, / 8. 1 call site.
#include "theseus.h"

void BitArray_ByteSize_002ac870(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(g.arg(0)) == 0)
    {
        g.ret(0);
        return;
    }
    g.ret(((g.read32(g.arg(0) + 4) + 31) & ~31u) >> 3);
}

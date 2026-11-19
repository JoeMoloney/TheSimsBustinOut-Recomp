// Bit a1 of the bit array at a0 (+0 word pointer, +4 bit count): 0 if a1 is not below the
// count (unsigned, all 64 bits), else whether the bit is set (3 call sites).
#include "theseus.h"

void BitArray_Test_002ac448(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    if (!(g.arg64(1) < static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(a + 4))))))
    {
        g.ret(0);
        return;
    }
    const uint32_t word = g.read32(g.read32(a) + ((g.arg(1) >> 5) << 2));
    g.ret((word >> (g.arg(1) & 31)) & 1);
}

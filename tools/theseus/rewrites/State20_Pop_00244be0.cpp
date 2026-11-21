// Pops the top 32-byte state from the stack at a0+0x38 back into a0 (count at +0x3B8 decremented and stored first). v0 = the first 8 bytes (all 64 bits, as ldl/ldr leave them). 2 call sites. Pairs with State20_Push_00244b88.
#include "theseus.h"

void State20_Pop_00244be0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0), n = g.read32(s + 0x3B8) - 1;
    g.write32(s + 0x3B8, n);
    const uint32_t from = s + (n << 5) + 0x38;
    const uint64_t w0 = g.read64(from), w1 = g.read64(from + 8), w2 = g.read64(from + 0x10), w3 = g.read64(from + 0x18);
    g.write64(s, w0);
    g.write64(s + 8, w1);
    g.write64(s + 0x10, w2);
    g.write64(s + 0x18, w3);
    g.ret64(w0);
}

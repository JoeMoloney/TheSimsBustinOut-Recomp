// Pushes the 32-byte state at a0 onto the stack at a0+0x38 (count at +0x3B8, no bound check): all 32 bytes are read before any is written, the count is stored last. Returns a0 + count * 32 (old count). 2 call sites.
#include "theseus.h"

void State20_Push_00244b88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0), n = g.read32(s + 0x3B8), base = s + (n << 5);
    const uint64_t w0 = g.read64(s), w1 = g.read64(s + 8), w2 = g.read64(s + 0x10), w3 = g.read64(s + 0x18);
    g.write64(base + 0x38, w0);
    g.write64(base + 0x40, w1);
    g.write64(base + 0x48, w2);
    g.write64(base + 0x50, w3);
    g.write32(s + 0x3B8, n + 1);
    g.ret(base);
}

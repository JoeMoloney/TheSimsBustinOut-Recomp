// Initialiser: +0 = 1; 8 bytes copied to +4 from a3 + 0xC8 when a3 != 0, else from a1; +0x14 = a2; +0x10 = a4 (t0) when a3 != 0, else -1; +0xC = a3. Returns a0. 2 call sites.
#include "theseus.h"

void Record18_Init_002f5b58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t r = g.arg(0);
    const bool have = g.arg64(3) != 0;  // movz/movn test the whole register
    g.write32(r, 1);
    g.write64(r + 4, g.read64(have ? g.arg(3) + 0xC8 : g.arg(1)));
    g.write32(r + 0x14, g.arg(2));
    g.write32(r + 0x10, have ? g.arg(4) : 0xFFFFFFFFu);
    g.write32(r + 0xC, g.arg(3));
    g.ret_copy_of_arg(0);
}

// Exchanges entry a1 of the word array at a0+0x270 with a2; returns the old entry. 2 call sites.
#include "theseus.h"

void Words270_Exchange_0026a4c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0) + 0x270 + (g.arg(1) << 2), old = g.read32(p);
    g.write32(p, g.arg(2));
    g.ret(old);
}

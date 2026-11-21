// Returns 1 when a0+0xC is nonzero and a0+0x64 is zero, else 0. 2 call sites. (The map joined other code after the `jr $ra`.)
#include "theseus.h"

void Field64_IsZeroIfC_001cefc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    g.ret(g.read32(obj + 0xC) != 0 && g.read32(obj + 0x64) == 0 ? 1 : 0);
}

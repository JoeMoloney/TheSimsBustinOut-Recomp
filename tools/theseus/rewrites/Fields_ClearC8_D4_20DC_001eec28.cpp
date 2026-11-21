// Zeroes the words at a0+0x20DC and a0+0xC8..+0xD4. 1 call site.
#include "theseus.h"

void Fields_ClearC8_D4_20DC_001eec28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x20DC, 0);
    g.write32(p + 0xC8, 0);
    g.write32(p + 0xCC, 0);
    g.write32(p + 0xD0, 0);
    g.write32(p + 0xD4, 0);
    g.ret_void();
}

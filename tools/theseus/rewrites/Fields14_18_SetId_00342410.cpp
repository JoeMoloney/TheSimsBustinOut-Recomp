// +0x30 = 0, +0x14 = a1, +0x18 = a1 (0 when a1 is -1). Returns -1. 3 call sites.
#include "theseus.h"

void Fields14_18_SetId_00342410(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0), id = g.arg(1);
    g.write32(p + 0x30, 0);
    g.write32(p + 0x14, id);
    g.write32(p + 0x18, g.sarg64(1) == -1 ? 0 : id);  // `bne` compares the whole register
    g.ret(0xFFFFFFFFu);
}

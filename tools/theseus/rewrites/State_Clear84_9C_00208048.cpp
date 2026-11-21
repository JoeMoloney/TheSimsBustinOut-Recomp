// Zeroes a0+0x84, +0x88, +0x8C, +0x90, +0x9C and the globals 0x3F9044, 0x3F9048, 0x3F9078 (in the original'\''s order). 1 call site.
#include "theseus.h"

void State_Clear84_9C_00208048(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x84, 0);
    g.write32(0x3F9044, 0);
    g.write32(p + 0x88, 0);
    g.write32(0x3F9048, 0);
    g.write32(p + 0x8C, 0);
    g.write32(p + 0x90, 0);
    g.write32(0x3F9078, 0);
    g.write32(p + 0x9C, 0);
    g.ret_void();
}

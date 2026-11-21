// Initialises a 0x24-byte object at a0: +0x20 = 0x3E47F0, +0 = -1, +0x14 = 0x3E47A0 (two vtable-like
// pointers), +4..+0x10 and +0x18, +0x1C = 0 (stores in the original's order); returns a0 (all 64 bits). 9 call sites.
#include "theseus.h"

void Object24_Init_002bf778(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x20, 0x003E47F0);
    g.write32(p + 0, 0xFFFFFFFF);
    g.write32(p + 0x14, 0x003E47A0);
    for (const uint32_t off : {0x4u, 0x8u, 0xCu, 0x10u, 0x18u, 0x1Cu})
        g.write32(p + off, 0);
    g.ret_copy_of_arg(0);
}

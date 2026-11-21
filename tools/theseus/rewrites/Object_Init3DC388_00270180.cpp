// Constructor: +0x1C = 0, +0x44 = 0x3DC388 (vtable), +0x2C down to +0x20 = 0xFF (4 words), +0x30..+0x3C = 0. Returns a0. 3 call sites.
#include "theseus.h"

void Object_Init3DC388_00270180(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x44, 0x3DC388);
    for (int32_t off = 0x2C; off >= 0x20; off -= 4)
        g.write32(p + static_cast<uint32_t>(off), 0xFF);
    g.write32(p + 0x30, 0);
    g.write32(p + 0x34, 0);
    g.write32(p + 0x38, 0);
    g.write32(p + 0x3C, 0);
    g.ret_copy_of_arg(0);
}

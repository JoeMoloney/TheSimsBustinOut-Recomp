// Constructor of a 0x20-byte object: words +0..+0x18 zero, +0x1C = 0x3F1CA0 (vtable). Returns
// a0. 6 call sites.
#include "theseus.h"

void Object20_Init3F1CA0_00345e30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    g.write32(obj, 0);
    g.write32(obj + 0x1C, 0x3F1CA0);
    g.write32(obj + 0x4, 0);
    g.write32(obj + 0x8, 0);
    g.write32(obj + 0xC, 0);
    g.write32(obj + 0x10, 0);
    g.write32(obj + 0x14, 0);
    g.write32(obj + 0x18, 0);
    g.ret_copy_of_arg(0);
}

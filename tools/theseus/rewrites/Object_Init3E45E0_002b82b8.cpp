// Constructor: +0 = 0x3E45E0 (vtable), halfword +0x1006 = 1, +0x100C = 0, +0x1008 = 0. Returns a0. The original then runs an empty 256-step loop (array elements with no-op constructors), which only leaves v1 = 1. 2 call sites.
#include "theseus.h"

void Object_Init3E45E0_002b82b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0x3E45E0);
    g.write16(p + 0x1006, 1);  // gate: equivalent write32: +0x1008 is stored later
    g.write32(p + 0x100C, 0);
    g.write32(p + 0x1008, 0);
    g.ret_copy_of_arg(0);
}

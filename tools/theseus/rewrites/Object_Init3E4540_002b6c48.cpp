// Constructor: +0 = 0x3E4540 (vtable), halfword +0x1092 = 1, +0x1098 = 0, +0x1094 = 0. Returns a0. The original then runs an empty 256-step loop (an array of 256 elements whose constructors compiled to nothing), which only leaves v1 = 1. 3 call sites.
#include "theseus.h"

void Object_Init3E4540_002b6c48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0x3E4540);
    g.write16(p + 0x1092, 1);  // gate: equivalent write32: +0x1094 is stored next
    g.write32(p + 0x1098, 0);
    g.write32(p + 0x1094, 0);
    g.ret_copy_of_arg(0);
}

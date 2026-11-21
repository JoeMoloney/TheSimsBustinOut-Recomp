// Constructor: +0 = 0x3E3F50 (vtable), +4..+0x27 = 0. (The original zeroes a 12-byte temporary in its stack frame, copies it to +0x10, copies +0x10 to +4, then zeroes +0x1C, +0x24, +0x20.) Returns a0. 2 call sites.
#include "theseus.h"

void Object_Init3E3F50_002ad088(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, 0x3E3F50);
    g.write64(p + 0x10, 0);
    g.write32(p + 0x18, 0);
    g.write64(p + 4, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x1C, 0);
    g.write32(p + 0x24, 0);
    g.write32(p + 0x20, 0);
    SET_GPR_U64(ctx, 3, 0);  // v1 and a1: the copied zeros, as the original leaves them; gate: equivalent
    SET_GPR_S32(ctx, 5, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

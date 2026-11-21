// Initialiser: +0xC = 0, +0x10 = 0xDEADBEEF (a guard value), +8, +0, +4 = 0. Returns a0 (v1 = 0xDEADBEEF). 1 call site.
#include "theseus.h"

void Object14_InitDeadBeef_00282808(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x10, 0xDEADBEEF);
    g.write32(p + 8, 0);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(0xDEADBEEF));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

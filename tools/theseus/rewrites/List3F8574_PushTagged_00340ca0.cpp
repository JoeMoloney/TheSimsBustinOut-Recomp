// Registers a0 in the global list 0x3F8574 ($gp - 0x757C): +4 = 0x3F1770 (vtable), +0 = the old head, head = a0. Returns a0 (v1 = the old head). 1 call site.
#include "theseus.h"

void List3F8574_PushTagged_00340ca0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t old = g.read32(0x3F8574);
    g.write32(g.arg(0) + 4, 0x3F1770);
    g.write32(g.arg(0), old);
    g.write32(0x3F8574, g.arg(0));
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(old));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

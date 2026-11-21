// Initialiser: +0xC = a1 (owner), +0x10 = a3, +4..+0xB = 8 bytes from a2, +0 = 1, +0x14 = the owner'\''s +0xE0 (read through the stored +0xC). Returns a0 (t0 = 1). 1 call site.
#include "theseus.h"

void Record18_InitFromOwner_002f5c18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0xC, g.arg(1));
    g.write32(p + 0x10, g.arg(3));
    g.write64(p + 4, g.read64(g.arg(2)));
    const uint32_t owner = g.read32(p + 0xC);
    const uint32_t value = g.read32(owner + 0xE0);
    g.write32(p, 1);
    g.write32(p + 0x14, value);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(owner));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, static_cast<int32_t>(value));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 8, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

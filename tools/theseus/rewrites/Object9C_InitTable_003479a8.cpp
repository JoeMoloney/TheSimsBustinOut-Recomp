// Initialiser: halfwords +0..+0x83 = 0 (+4..+0x83 in a loop of 16 x 4, then +0 and +2), +0x94 = 50, +0x98 = -1, +0x84 = 3, +0x8C = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object9C_InitTable_003479a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const uint64_t self = g.arg64(0);
    for (uint32_t off = 4; off < 0x84; off += 2)
        g.write16(p + off, 0);
    g.write32(p + 0x94, 50);
    g.write32(p + 0x98, 0xFFFFFFFFu);
    g.write32(p + 0x84, 3);
    g.write32(p + 0x8C, 0);
    g.write16(p, 0);
    g.write16(p + 2, 0);
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, static_cast<int32_t>(p));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, 3);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 2, self);  // daddu v0, a1 (= the entry a0)
    g.ret_void();
}

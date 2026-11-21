// Constructor: +0x24 = 0x3E5B88 (vtable), +0x20 = 0x900DBEEF (a guard value), halfwords +0x10/+0x12 = 0x40, bytes +0x18 = 1, +0x1A = 0x20, +0x19/+0x1B = 0, halfwords +0x14/+0x16 = 0, words +0/+4/+8/+0xC/+0x1C = 0. Returns a0. 1 call site.
#include "theseus.h"

void Object28_Init3E5B88_002c2eb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x24, 0x3E5B88);
    g.write16(p + 0x10, 0x40);
    g.write8(p + 0x18, 1);
    g.write8(p + 0x1A, 0x20);
    g.write32(p + 0x20, 0x900DBEEF);
    g.write32(p, 0);
    g.write32(p + 4, 0);
    g.write32(p + 8, 0);
    g.write16(p + 0x12, 0x40);
    g.write8(p + 0x19, 0);
    g.write8(p + 0x1B, 0);
    g.write16(p + 0x14, 0);
    g.write16(p + 0x16, 0);
    g.write32(p + 0xC, 0);
    g.write32(p + 0x1C, 0);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(0x900DBEEF));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, 0x20);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, 0x40);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

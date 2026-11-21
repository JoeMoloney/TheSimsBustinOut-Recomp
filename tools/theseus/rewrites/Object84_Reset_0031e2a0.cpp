// Resets an object: +0x5C = 0xFFFF0000, +0x58/+0x50/+0x54 = -1, halfword +0x4C = 0, every other word from +0 to +0x80 = 0 (in the original'\''s order). Returns a0. 2 call sites.
#include "theseus.h"

void Object84_Reset_0031e2a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p + 0x5C, 0xFFFF0000u);
    g.write32(p + 0x58, 0xFFFFFFFFu);
    static const uint8_t kZero1[] = {0x04, 0x08, 0x0C, 0x18, 0x1C, 0x30, 0x34, 0x38, 0x3C, 0x40, 0x44, 0x48};
    for (uint8_t off : kZero1)
        g.write32(p + off, 0);
    g.write16(p + 0x4C, 0);
    g.write32(p + 0x50, 0xFFFFFFFFu);
    g.write32(p + 0x54, 0xFFFFFFFFu);
    static const uint8_t kZero2[] = {0x60, 0x64, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x00, 0x20, 0x24, 0x28, 0x2C};
    for (uint8_t off : kZero2)
        g.write32(p + off, 0);
    SET_GPR_S32(ctx, 3, -1);  // v1, as the original leaves it; gate: equivalent
    g.ret_copy_of_arg(0);
}

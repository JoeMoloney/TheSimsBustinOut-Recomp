// When a0 has a target object (+0x4AC) and a1 != 0: copies a1'\''s mode (+0x14) to a0+0x480 if it differs, then sets the target'\''s +0x90 = (mode == 1) and +0x88 = whether that flag changed. v0: 0 without a target, the target with a1 == 0, mode ^ 1 when the mode was unchanged, else the changed flag. 2 call sites.
#include "theseus.h"

void Mode480_Sync_00175ef8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0), target = g.read32(obj + 0x4AC);
    if (target == 0)
    {
        g.ret(0);
        return;
    }
    if (g.arg64(1) == 0)
    {
        g.ret(target);
        return;
    }
    const uint32_t mode = g.read32(g.arg(1) + 0x14);
    if (g.read32(obj + 0x480) == mode)
    {
        g.ret(mode ^ 1);
        return;
    }
    g.write32(obj + 0x480, mode);
    const uint32_t is_one = mode == 1;
    const uint32_t t = g.read32(obj + 0x4AC), old = g.read32(t + 0x90);
    g.write32(t + 0x90, is_one);
    const uint32_t changed = is_one != old;
    g.write32(t + 0x88, changed);
    g.ret(changed);
}

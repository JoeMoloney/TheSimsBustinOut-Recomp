// Pushes a1 onto the free list at a0+0x10C (link a1+0), then clears the first of a0'\''s 64 slot words (+0..+0xFC) that holds a1 (compared as a whole register). Returns a0 + 0x104 when slot 0 matched, the slot'\''s address for a later one, 0x100 when none did. 1 call site.
#include "theseus.h"

void Slots64_ReleaseToPool_00365790(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t table = g.arg(0), item = g.arg(1);
    const int64_t item64 = g.sarg64(1);
    g.write32(item, g.read32(table + 0x10C));
    g.write32(table + 0x10C, item);
    if (static_cast<int32_t>(g.read32(table)) == item64)
    {
        g.write32(table, 0);
        g.ret(table + 0x104);
        return;
    }
    for (uint32_t i = 1; i < 64; ++i)
    {
        const uint32_t slot = table + (i << 2);
        if (static_cast<int32_t>(g.read32(slot)) == item64)
        {
            g.write32(slot, 0);
            g.ret(slot);
            return;
        }
    }
    g.ret(0x100);  // the `sll` in the exit branch's delay slot: 64 * 4
}

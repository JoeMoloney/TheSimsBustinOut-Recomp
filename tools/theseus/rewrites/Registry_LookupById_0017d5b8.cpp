// Looks up id a0 (all 64 bits) in the list reached from the global at 0x3FA5C0 (+4, then +4;
// node: +0x10 next, +0x18 value, +0x1C id) and returns the value; -1 if the global, the
// intermediate object or the id is missing (5 call sites).
#include "theseus.h"

void Registry_LookupById_0017d5b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t root = g.read32(0x3FA5C0);
    if (root == 0)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    const uint32_t list = g.read32(root + 4);
    if (list == 0)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    for (uint32_t node = g.read32(list + 4); node != 0; node = g.read32(node + 0x10))
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(node + 0x1C))) == g.sarg64(0))
        {
            g.ret(g.read32(node + 0x18));
            return;
        }
    g.ret(0xFFFFFFFFu);
}

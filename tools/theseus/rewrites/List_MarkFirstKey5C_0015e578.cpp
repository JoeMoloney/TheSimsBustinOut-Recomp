// Finds the first node of the list [a0] (+0 object, +8 next) whose object'\''s +8 equals a1'\''s +0x5C and sets that object'\''s +4 to 1 (returns 1). Not found: v0 = the last key read; empty list: v0 untouched. 1 call site.
#include "theseus.h"

void List_MarkFirstKey5C_0015e578(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(0));
    if (node == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t key = g.read32(g.arg(1) + 0x5C);
    uint32_t last = 0;  // gate: equivalent 1: overwritten in the loop before it is read
    for (;;)
    {
        const uint32_t obj = g.read32(node);
        last = g.read32(obj + 8);
        if (last == key)
        {
            g.write32(obj + 4, 1);
            g.ret(1);
            return;
        }
        node = g.read32(node + 8);
        if (node == 0)
            break;
    }
    g.ret(last);
}

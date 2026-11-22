// For every node of the list [a0] (+0 object, +8 next): zeroes the object'\''s +0xBC. v0 = the last object (untouched for an empty list). 1 call site. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void List_ClearObjBC_001498b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(0));
    if (node == 0)
    {
        g.ret_void();
        return;
    }
    uint32_t obj = 0;  // gate: equivalent 1: overwritten in the loop before it is read
    do
    {
        obj = g.read32(node);
        g.write32(obj + 0xBC, 0);
        node = g.read32(node + 8);
    } while (node != 0);
    SET_GPR_S32(ctx, 4, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(obj);
}

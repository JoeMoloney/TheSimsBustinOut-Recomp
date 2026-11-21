// For every node of the list [a0] (+0 object, +8 next): sets the object'\''s +0 to 1 when its +8 equals a1 (whole register). v0 = the last key read (untouched for an empty list); a2 = 1. 1 call site.
#include "theseus.h"

void List_MarkObjKey8_0015e620(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    SET_GPR_S32(ctx, 6, 1);  // scratch register for callers (not ABI-compared); gate: equivalent
    uint32_t node = g.read32(g.arg(0));
    if (node == 0)
    {
        g.ret_void();
        return;
    }
    uint32_t key = 0;
    do
    {
        const uint32_t obj = g.read32(node);
        key = g.read32(obj + 8);
        if (static_cast<int32_t>(key) == g.sarg64(1))
            g.write32(obj, 1);
        node = g.read32(node + 8);
    } while (node != 0);
    SET_GPR_S32(ctx, 4, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(key);
}

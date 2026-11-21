// find_if over a list range: from the node *a1 up to (not including) *a2 (next +0), the first node whose +8 equals *a3 (re-read each step); *a0 = that node, or *a2 when none matches. Returns a0 (v1 = the result). (The original keeps the iterator in its stack frame.) 1 call site.
#include "theseus.h"

void List_FindKey8Iter_0035cf90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t it = g.read32(g.arg(1));
    const uint32_t end = g.read32(g.arg(2));
    while (it != end && g.read32(it + 8) != g.read32(g.arg(3)))
        it = g.read32(it);
    g.write32(g.arg(0), it);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(it));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}

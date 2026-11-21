// a1 selects a flag (0 -> bit 0, 1 -> bit 3, other -> none); clears it in +0x344 of the object
// (+0x1C) of every node in the list at [a0+4] (next +0x10). v0 = the last word written, or
// (empty list) 1, or untouched for a1 == 0. 4 call sites.
#include "theseus.h"

void List4_ClearFlag344_0017cb60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t which = g.arg64(1);
    const uint32_t flag = which == 0 ? 1 : which == 1 ? 8 : 0;
    bool have_v0 = which != 0;
    uint32_t v0 = 1;
    for (uint32_t node = g.read32(g.arg(0) + 4); node != 0; node = g.read32(node + 0x10))
    {
        const uint32_t field = g.read32(node + 0x1C) + 0x344;
        v0 = g.read32(field) & ~flag;
        g.write32(field, v0);  // gate: equivalent write16: clearing bits 0/3 leaves the upper half as it was
        have_v0 = true;
    }
    if (have_v0)
        g.ret(v0);
    else
        g.ret_void();
}

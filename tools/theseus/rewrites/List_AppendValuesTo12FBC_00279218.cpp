// Appends the value (+0) of every node of the list [a1] (next +8) to the word array at a0+0x12FBC, indexed by the count at a0+0x157BC (re-read and incremented each time, no bound check). v0 = the last slot written, or 0x10000 for an empty list. 1 call site.
#include "theseus.h"

void List_AppendValuesTo12FBC_00279218(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(1));
    if (node == 0)
    {
        g.ret(0x10000);
        return;
    }
    const uint32_t base = g.arg(0) + 0x12FBC, count_at = g.arg(0) + 0x157BC;
    uint32_t slot = 0;  // gate: equivalent 1: overwritten in the loop before it is read
    do
    {
        slot = base + (g.read32(count_at) << 2);
        g.write32(slot, g.read32(node));
        g.write32(count_at, g.read32(count_at) + 1);
        node = g.read32(node + 8);
    } while (node != 0);
    g.ret(slot);
}

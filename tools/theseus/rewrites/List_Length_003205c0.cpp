// Length of the singly linked list whose head pointer is at a0 (each node's +0 is the next
// node, 0 ends the list). 9 call sites.
#include "theseus.h"

void List_Length_003205c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(0));
    uint32_t n = 0;
    while (node != 0)
    {
        node = g.read32(node);
        ++n;
    }
    g.ret(n);
}

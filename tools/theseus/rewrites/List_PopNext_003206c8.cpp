// Unlinks the first node of the list whose head is *a0 (link +0): head = head->next. Returns the new head (0 when the list was empty). 1 call site.
#include "theseus.h"

void List_PopNext_003206c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.read32(g.arg(0));
    if (head == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t next = g.read32(head);
    g.write32(g.arg(0), next);
    g.ret(next);
}

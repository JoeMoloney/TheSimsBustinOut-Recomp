// Pushes a1 onto the list whose head is at a0+4 (link at a1+0). Returns the old head. 1 call site.
#include "theseus.h"

void List4_Push_00225ba8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.arg(0) + 4, old = g.read32(head);
    g.write32(g.arg(1), old);
    g.write32(head, g.arg(1));
    g.ret(old);
}

// Pushes a1 onto the list whose head is at a0+0x408 (link at a1+0x24). Returns the old head. 1 call site.
#include "theseus.h"

void List408_Push_0035e8d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.arg(0) + 0x408, old = g.read32(head);
    g.write32(g.arg(1) + 0x24, old);
    g.write32(head, g.arg(1));
    g.ret(old);
}

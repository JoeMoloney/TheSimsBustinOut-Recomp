// Pushes a1 onto the list whose head is at a0+0x17FC8 (link at a1+0x28). Returns the old head. 2 call sites.
#include "theseus.h"

void List17FC8_Push_0027bd30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t head = g.arg(0) + 0x17FC8, old = g.read32(head);
    g.write32(g.arg(1) + 0x28, old);
    g.write32(head, g.arg(1));
    g.ret(old);
}

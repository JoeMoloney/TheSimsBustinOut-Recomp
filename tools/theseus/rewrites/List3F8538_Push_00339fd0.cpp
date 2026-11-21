// Pushes a0 onto the global list 0x3F8538 ($gp - 0x75B8; link at +4). Returns the old head. 1 call site. Pairs with List3F8538_AssignUniqueId_0033a0b8.
#include "theseus.h"

void List3F8538_Push_00339fd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t old = g.read32(0x3F8538);
    g.write32(g.arg(0) + 4, old);
    g.write32(0x3F8538, g.arg(0));
    g.ret(old);
}

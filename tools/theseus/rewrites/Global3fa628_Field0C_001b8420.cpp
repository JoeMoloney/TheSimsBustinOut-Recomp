// Word +0xC of the object the global at 0x3FA628 points to (2 call sites).
#include "theseus.h"

void Global3fa628_Field0C_001b8420(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(0x3FA628) + 0xC));
}

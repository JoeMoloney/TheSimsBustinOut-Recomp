// Word +0x10 of the object at +0x54 of the object at +0x4C of a0 (2 call sites).
#include "theseus.h"

void Field4C_54_10_001df7c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.read32(g.arg(0) + 0x4C) + 0x54) + 0x10));
}

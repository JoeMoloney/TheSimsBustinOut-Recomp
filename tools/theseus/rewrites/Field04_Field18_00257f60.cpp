// Word +0x18 of the object at +0x4 of a0 (2 call sites).
#include "theseus.h"

void Field04_Field18_00257f60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.arg(0) + 4) + 0x18));
}

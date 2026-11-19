// Returns field +0x1C of the object that +0x18 of a0 points to (41 call sites).
#include "theseus.h"

void Field18_Field1C_0031e528(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t inner = g.read32(g.arg(0) + 0x18);
    g.ret(g.read32(inner + 0x1C));
}

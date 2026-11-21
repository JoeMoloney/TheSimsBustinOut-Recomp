// [[[a0+0x80]+0x18]+0x1C], or 0 if [a0+0x80] is null. 1 call site.
#include "theseus.h"

void Field80_Field18_Field1C_0019bd58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.read32(g.arg(0) + 0x80);
    g.ret(p ? g.read32(g.read32(p + 0x18) + 0x1C) : 0);
}

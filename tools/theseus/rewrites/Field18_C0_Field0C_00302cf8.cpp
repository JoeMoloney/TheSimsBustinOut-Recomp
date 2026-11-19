// Word +0xC of the object at +0xC0 of the object at +0x18 of a1; 0 if the +0xC0 pointer is
// null (3 call sites).
#include "theseus.h"

void Field18_C0_Field0C_00302cf8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t inner = g.read32(g.read32(g.arg(1) + 0x18) + 0xC0);
    g.ret(inner == 0 ? 0 : g.read32(inner + 0xC));
}

// Bit 0 of word +4 of the object at +0xC0 of the object at +0x18 of a0; 0 if the +0xC0
// pointer is null (5 call sites).
#include "theseus.h"

void Field18_C0_Bit0_0031e418(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t outer = g.read32(g.arg(0) + 0x18);
    const uint32_t inner = g.read32(outer + 0xC0);
    if (inner == 0)
        g.ret(0);
    else
        g.ret(g.read32(inner + 4) & 1);  // gate: equivalent read16: only bit 0 is used
}

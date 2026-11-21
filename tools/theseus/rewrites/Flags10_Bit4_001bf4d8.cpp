// Bit 4 of the word at a0+0x10 (0 or 1). 1 call site.
#include "theseus.h"

void Flags10_Bit4_001bf4d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((static_cast<int32_t>(g.read32(g.arg(0) + 0x10)) >> 4) & 1);  // gate: equivalent read16: only bit 4 is used
}

// Bit 0 of the word at +0x10 of a0 (2 call sites).
#include "theseus.h"

void Field10_Bit0_001bf488(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x10) & 1);  // gate: equivalent read16: only bit 0 is used
}

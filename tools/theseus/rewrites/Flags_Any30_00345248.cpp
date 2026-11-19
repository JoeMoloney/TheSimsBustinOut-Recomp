// True if bit 4 or 5 of the word at a0 is set (14 call sites).
#include "theseus.h"

void Flags_Any30_00345248(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((g.read32(g.arg(0)) & 0x30) != 0 ? 1 : 0);  // gate: equivalent read16: only bits 4-5 are tested
}

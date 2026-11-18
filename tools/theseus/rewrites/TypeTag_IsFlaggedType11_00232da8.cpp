// Is the tag word at a0 type 11 (low 15 bits) with its high flag (bit 15) set? (40 callers)
// Replaces FUN_00232da8; FUN_00232da8/dd0/df8 are the same check for types 11, 24 and 23.
#include "theseus.h"

void TypeTag_IsFlaggedType11_00232da8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t tag = g.read32(g.arg(0));  // gate: equivalent read16: only the low 16 bits are used
    g.ret((tag & 0x7FFF) == 0xb && (tag & 0x8000) != 0);
}

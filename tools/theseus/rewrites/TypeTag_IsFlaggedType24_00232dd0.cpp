// Is the tag word at a0 type 24 (low 15 bits) with its high flag (bit 15) set? (40 callers)
// Replaces FUN_00232dd0; FUN_00232da8/dd0/df8 are the same check for types 11, 24 and 23.
#include "theseus.h"

void TypeTag_IsFlaggedType24_00232dd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t tag = g.read32(g.arg(0));  // gate: equivalent read16: only the low 16 bits are used
    g.ret((tag & 0x7FFF) == 0x18 && (tag & 0x8000) != 0);  // gate: equivalent 0x8001: type 24 has bit 0 clear, so tag & 0x8001 tests bit 15 alone
}

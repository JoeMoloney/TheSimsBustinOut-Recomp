// The 16-bit field +0x8 of a0 if a1 is 1 or 2, else the one at +0x14 (8 call sites).
#include "theseus.h"

void Field8or14_ByMode_003456f8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const bool one_or_two = g.arg(1) - 1 < 2;  // `addiu -1` + `sltiu 2`
    g.ret(g.read16(g.arg(0) + (one_or_two ? 0x8 : 0x14)));
}

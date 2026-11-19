// True if word +0x8 of slot a1 (1-based) of the pointer table at +0x130 of a0 is nonzero
// (8 call sites).
#include "theseus.h"

void Slots130_NonEmpty_002f0568(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t slot = g.read32(g.arg(0) + ((g.arg(1) - 1) << 2) + 0x130);
    g.ret(g.read32(slot + 8) != 0 ? 1 : 0);
}

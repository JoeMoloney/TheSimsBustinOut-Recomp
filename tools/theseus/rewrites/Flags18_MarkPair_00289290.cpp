// Sets flag 0x20 in a1+0x18 and flag 0x40 in a0+0x18. Returns a1'\''s new flags (v1 = a0'\''s). 1 call site.
#include "theseus.h"

void Flags18_MarkPair_00289290(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t f1 = g.read32(g.arg(1) + 0x18) | 0x20;
    g.write32(g.arg(1) + 0x18, f1);  // gate: equivalent write16: only bit 5 changes
    const uint32_t f0 = g.read32(g.arg(0) + 0x18) | 0x40;
    g.write32(g.arg(0) + 0x18, f0);  // gate: equivalent write16: only bit 6 changes
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(f0));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(f1);
}

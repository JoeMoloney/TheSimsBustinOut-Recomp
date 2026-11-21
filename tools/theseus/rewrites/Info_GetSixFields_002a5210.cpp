// Fills six outputs from the object a0: *a3 = [[a0+0x20]+4] >> 13; *a5 (t1) = [[a0+0x24]+4] >> 13 when bit 0 of a0+8 is set, else 0; *a1 = a0+0; *a2 = a0+4; *a4 (t0) = a0+0x30; *a6 (t2) = a0+0x34 (in the original'\''s order). Returns the +0x30 word (v1 = the +0x34 word). 1 call site.
#include "theseus.h"

void Info_GetSixFields_002a5210(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(g.arg(3), g.read32(g.read32(p + 0x20) + 4) >> 13);
    g.write32(g.arg(5), 0);
    if (g.read32(p + 8) & 1)
        g.write32(g.arg(5), g.read32(g.read32(p + 0x24) + 4) >> 13);
    g.write32(g.arg(1), g.read32(p));
    g.write32(g.arg(2), g.read32(p + 4));
    const uint32_t w30 = g.read32(p + 0x30);
    g.write32(g.arg(4), w30);
    const uint32_t w34 = g.read32(p + 0x34);
    g.write32(g.arg(6), w34);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(w34));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(w30);
}

// Copies 4 words to a0+0x4C from the global table 0x40B310 (a1 != 0, whole register) or 0x40B320 (a1 == 0). Returns the last word (v1/a0/a1 as the original leaves them). 1 call site.
#include "theseus.h"

void Words4_CopyFromTable40B3_001c2090(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg64(1) != 0 ? 0x40B310 : 0x40B320, dst = g.arg(0) + 0x4C;
    uint32_t w = 0;
    for (uint32_t i = 0; i < 16; i += 4)
    {
        w = g.read32(src + i);
        g.write32(dst + i, w);
    }
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(src + 16));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, static_cast<int32_t>(dst + 16));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(w);
}

// Copies 0x1C bytes from a1 to a0 (all read first). Returns 1 (v1/a2/a3/t0 hold the copied data, as the original leaves them). 1 call site. (The map joined two getters after the `jr $ra`.)
#include "theseus.h"

void Bytes1C_CopyRetOne_002c2e00(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg(1), dst = g.arg(0);
    const uint64_t w0 = g.read64(src), w1 = g.read64(src + 8), w2 = g.read64(src + 0x10);
    const uint32_t w3 = g.read32(src + 0x18);
    g.write64(dst, w0);
    g.write64(dst + 8, w1);
    g.write64(dst + 0x10, w2);
    g.write32(dst + 0x18, w3);
    SET_GPR_U64(ctx, 3, w0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 6, w1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 7, w2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 8, static_cast<int32_t>(w3));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(1);
}

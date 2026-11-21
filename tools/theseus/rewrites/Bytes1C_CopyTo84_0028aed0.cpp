// Copies 0x1C bytes from a1 to a0+0x84 (all read first). v0 = the first 8 bytes (64 bits); v1/a2/a3 the rest, as the original leaves them. 1 call site.
#include "theseus.h"

void Bytes1C_CopyTo84_0028aed0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg(1), dst = g.arg(0) + 0x84;
    const uint64_t w0 = g.read64(src), w1 = g.read64(src + 8), w2 = g.read64(src + 0x10);
    const uint32_t w3 = g.read32(src + 0x18);
    g.write64(dst, w0);
    g.write64(dst + 8, w1);
    g.write64(dst + 0x10, w2);
    g.write32(dst + 0x18, w3);
    SET_GPR_U64(ctx, 3, w1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 6, w2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 7, static_cast<int32_t>(w3));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret64(w0);
}

// Copies 0x1C bytes from a1 to a0 (all read before any is written). Returns a0. 4 call sites.
#include "theseus.h"

void Bytes1C_Copy_002d1298(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0), src = g.arg(1);
    const uint64_t w0 = g.read64(src), w1 = g.read64(src + 8), w2 = g.read64(src + 0x10);
    const uint32_t w3 = g.read32(src + 0x18);
    g.write64(dst, w0);
    g.write64(dst + 8, w1);
    g.write64(dst + 0x10, w2);
    g.write32(dst + 0x18, w3);
    // one caller (0x10e2ac) reads a3 and t0 afterwards: leave them as the original's loads do
    SET_GPR_U64(ctx, 7, w2);  // gate: equivalent 8: a scratch register for that caller (not in the ABI compare)
    SET_GPR_S32(ctx, 8, w3);  // gate: equivalent 9: same
    g.ret_copy_of_arg(0);
}

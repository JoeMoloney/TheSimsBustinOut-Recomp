// Copies a2 records of 0x1C bytes from a1 to a0 (each record read whole, then written). Nothing for a2 == 0 (whole register; a count whose low 32 bits are 0 copies 2^32 records, as in the original). Returns -1. 1 call site.
#include "theseus.h"

void Records1C_Copy_0027e1b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(2) == 0)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    uint32_t dst = g.arg(0), src = g.arg(1), left = g.arg(2);
    uint64_t w0 = 0, w1 = 0, w2 = 0;  // gate: equivalent 1: overwritten in the loop before it is read
    uint32_t w3 = 0;  // gate: equivalent 1: overwritten in the loop before it is read
    do
    {
        w0 = g.read64(src); w1 = g.read64(src + 8); w2 = g.read64(src + 0x10);
        w3 = g.read32(src + 0x18);
        g.write64(dst, w0); g.write64(dst + 8, w1); g.write64(dst + 0x10, w2);
        g.write32(dst + 0x18, w3);
        src += 0x1C;
        dst += 0x1C;
    } while (--left != 0);
    SET_GPR_U64(ctx, 6, w0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 7, w1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 8, w2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 9, static_cast<int32_t>(w3));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0xFFFFFFFFu);
}

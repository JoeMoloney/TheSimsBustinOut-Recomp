// Copies a2 records of 0x3C bytes from a1 to a0 (per record: the first 32 bytes read then written, then the other 28). Nothing for a2 == 0 (whole register); a count whose low 32 bits are 0 copies 2^32 records, as in the original. Returns -1. 2 call sites.
#include "theseus.h"

void Records3C_Copy_0027e5f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(2) == 0)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    uint32_t dst = g.arg(0), src = g.arg(1), left = g.arg(2);
    uint64_t w0 = 0, w1 = 0, w2 = 0, w3 = 0;
    uint32_t tail = 0;
    do
    {
        w0 = g.read64(src); w1 = g.read64(src + 8); w2 = g.read64(src + 0x10); w3 = g.read64(src + 0x18);
        g.write64(dst, w0); g.write64(dst + 8, w1); g.write64(dst + 0x10, w2); g.write64(dst + 0x18, w3);
        w0 = g.read64(src + 0x20); w1 = g.read64(src + 0x28); w2 = g.read64(src + 0x30);
        tail = g.read32(src + 0x38);
        g.write64(dst + 0x20, w0); g.write64(dst + 0x28, w1); g.write64(dst + 0x30, w2);
        g.write32(dst + 0x38, tail);
        src += 0x3C;
        dst += 0x3C;
    } while (--left != 0);
    // a2/a3/t0/t1 as the last record's loads leave them, for callers that read them
    SET_GPR_U64(ctx, 6, w0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 7, w1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 8, w2);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 9, tail);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0xFFFFFFFFu);
}

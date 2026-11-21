// Initialiser: +0 = a1, +0x10 = a3, +4 = -1, +8 = a2, +0xC = first byte of *a2 xor 0xAAAAAAAA (a check value). Returns that value (v1 = 0xAAAAAAAA). 1 call site.
#include "theseus.h"

void Cursor_Init_002ec068(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write32(p, g.arg(1));
    g.write32(p + 0x10, g.arg(3));
    g.write32(p + 4, 0xFFFFFFFFu);
    g.write32(p + 8, g.arg(2));
    const uint32_t check = g.read8(g.arg(2)) ^ 0xAAAAAAAAu;
    g.write32(p + 0xC, check);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(0xAAAAAAAAu));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(check);
}

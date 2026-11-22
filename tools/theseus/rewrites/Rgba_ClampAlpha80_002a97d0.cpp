// Clamps the alpha byte (+3) of a1 RGBA pixels at a0 to at most 0x80 (the PS2'\''s 1.0). v0 = the last (alpha < 0x81) test, untouched for a1 <= 0; v1 = 0x80. 1 call site. (The count tests follow the generated code, which compares the low 32 bits.)
#include "theseus.h"

void Rgba_ClampAlpha80_002a97d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    SET_GPR_S32(ctx, 3, 0x80);  // scratch register for callers (not ABI-compared); gate: equivalent
    int32_t n = static_cast<int32_t>(g.arg(1));
    if (n <= 0)
    {
        g.ret_void();
        return;
    }
    uint32_t p = g.arg(0), below = 0;  // gate: equivalent 1: below is overwritten in the loop (n > 0 here)
    do
    {
        below = g.read8(p + 3) < 0x81;
        if (!below)
            g.write8(p + 3, 0x80);
        --n;
        p += 4;
    } while (n > 0);
    g.ret(below);
}

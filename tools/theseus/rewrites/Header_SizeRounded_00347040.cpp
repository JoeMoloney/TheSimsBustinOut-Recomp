// ((byte +6 + byte +7 + 3) & 0x3FC) * 2 + 0x10 of a0: a header plus 16-bit entries, rounded
// (4 call sites).
#include "theseus.h"

void Header_SizeRounded_00347040(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    g.ret(((g.read8(a + 6) + g.read8(a + 7) + 3) & 0x3FC) * 2 + 0x10);
}

// Classifies the 16-bit character a0 (signed): 0x100..0xFFF -> 1, 0x1000..0x1FFF -> 2,
// 0x2000..0x2328 -> 3, anything else (incl. below 0x100 and negative) -> 0. 1 call site.
#include "theseus.h"

void Char16_Range_002c7438(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t c = static_cast<int16_t>(g.arg(0));
    if ((c & 0xFFFF) < 0x100)
        g.ret(0);
    else if (static_cast<uint32_t>(c - 0x100) < 0xF00)
        g.ret(1);
    else if (static_cast<uint32_t>(c - 0x2000) < 0x329)
        g.ret(3);
    else
        g.ret(static_cast<uint32_t>(c - 0x1000) < 0x1000 ? 2 : 0);  // gate: equivalent <=,0x1001: c == 0x2000 returned 3 above
}

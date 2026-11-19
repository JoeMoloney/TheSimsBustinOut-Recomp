// True for a 16-bit character (low half of a0) that is white space: 9..13 (tab, LF, VT, FF, CR)
// or 32 (space). 8 call sites.
#include "theseus.h"

void Char16_IsSpace_0013ef70(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t c = g.arg(0) & 0xFFFF;
    g.ret(c == 0x20 || c - 9 < 5 ? 1 : 0);
}

// wcscpy for 16-bit strings: copies a1 to a0 including the terminator; returns a0 (all 64 bits). 13 call sites.
#include "theseus.h"

void Str16_Copy_00349c38(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1);
    for (uint16_t c = g.read16(src); c != 0; c = g.read16(src))  // gate: equivalent read32: truncated to uint16_t
    {
        g.write16(dst, c);  // gate: equivalent write32: the upper half is 0 and is overwritten by the next character or the terminator
        src += 2;
        dst += 2;
    }
    g.write16(dst, 0);
    g.ret_copy_of_arg(0);
}

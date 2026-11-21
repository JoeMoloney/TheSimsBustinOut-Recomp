// *a0 = *a1 (one byte); returns a0 (a char_traits<char>::assign-style helper). 5 call sites
// (2 identical copies).
// theseus: copies 0x375a68
#include "theseus.h"

void Byte_Copy_00375a30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write8(g.arg(0), g.read8(g.arg(1)));  // gate: equivalent read16: write8 keeps only the low byte
    g.ret_copy_of_arg(0);
}

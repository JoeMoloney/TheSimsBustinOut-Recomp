// Address a0 + 0x10 + 2 * (byte at a0+7): the 16-bit entries after a 16-byte header (14 call sites).
#include "theseus.h"

void ArrayAfterHeader_00347028(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    g.ret(a + 0x10 + 2u * g.read8(a + 7));
}

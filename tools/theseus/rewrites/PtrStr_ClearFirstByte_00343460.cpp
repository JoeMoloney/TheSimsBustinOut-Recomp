// Clears the first byte of the buffer *a0 and returns *a0. 19 call sites.
#include "theseus.h"

void PtrStr_ClearFirstByte_00343460(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t buffer = g.read32(g.arg(0));
    g.write8(buffer, 0);
    g.ret(buffer);
}

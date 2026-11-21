// Clears the first 16-bit character of the buffer *a0 and returns *a0. 17 call sites.
#include "theseus.h"

void PtrStr_ClearFirstHalf_00343ad0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t buffer = g.read32(g.arg(0));
    g.write16(buffer, 0);
    g.ret(buffer);
}

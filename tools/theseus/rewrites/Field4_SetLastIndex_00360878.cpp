// Sets a0+4 = (the word before the buffer **a0, i.e. its count) - 1, or -1 when there is no buffer. Returns it. 1 call site.
#include "theseus.h"

void Field4_SetLastIndex_00360878(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t buf = g.read32(g.read32(g.arg(0)));
    const uint32_t value = (buf == 0 ? 0 : g.read32(buf - 4)) - 1;
    g.write32(g.arg(0) + 4, value);
    g.ret(value);
}

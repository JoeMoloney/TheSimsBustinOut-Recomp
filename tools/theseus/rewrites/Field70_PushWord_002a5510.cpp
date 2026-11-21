// Stores a1 at the write pointer [a0+0x70], advances it by 4 and returns the new pointer. 10 call sites.
#include "theseus.h"

void Field70_PushWord_002a5510(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.read32(g.arg(0) + 0x70);
    g.write32(p, g.arg(1));
    g.write32(g.arg(0) + 0x70, p + 4);
    g.ret(p + 4);
}

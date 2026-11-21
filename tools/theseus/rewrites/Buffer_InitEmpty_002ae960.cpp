// Initialises a {data +0, capacity +4} buffer: a0+4 = a2, a0+0 = a1, and terminates the empty
// string at a1. 5 call sites. (The map joined the stack-frame epilogues after the `jr $ra`.)
#include "theseus.h"

void Buffer_InitEmpty_002ae960(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, g.arg(2));
    g.write32(g.arg(0), g.arg(1));
    g.write8(g.arg(1), 0);
    g.ret_void();
}

// Returns a0 + 0x10. 1 call site. (The map joined two small setters after the `jr $ra`.)
#include "theseus.h"

void Addr_Plus10_002567a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.arg(0) + 0x10);
}

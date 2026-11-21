// Does nothing (`jr $ra`). 1 call site.
#include "theseus.h"

void Empty_001b6fb8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret_void();
}

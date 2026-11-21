// The word at the global gp-0x768C. 1 call site.
#include "theseus.h"

void GpGlobal768C_002f26c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.gp() - 0x768C));
}

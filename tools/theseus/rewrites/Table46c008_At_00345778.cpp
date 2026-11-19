// Word [a1][a0] of the global table at 0x46C008 (64 words per row). 7 call sites.
#include "theseus.h"

void Table46c008_At_00345778(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x46C008 + (g.arg(0) << 2) + (g.arg(1) << 8)));
}

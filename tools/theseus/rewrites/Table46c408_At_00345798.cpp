// Word [a1][a0] of the global table at 0x46C408 (rows of 20 bytes). 3 call sites.
#include "theseus.h"

void Table46c408_At_00345798(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x46C408 + (g.arg(0) << 2) + g.arg(1) * 20));
}

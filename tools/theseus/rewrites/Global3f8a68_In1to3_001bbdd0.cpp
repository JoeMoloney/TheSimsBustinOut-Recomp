// True if the global at 0x3F8A68 is 1, 2 or 3 (4 call sites).
#include "theseus.h"

void Global3f8a68_In1to3_001bbdd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x3F8A68) - 1 < 3 ? 1 : 0);
}

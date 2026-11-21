// Maps 1 -> 1, 2 -> 4, 3 -> 3, 4 -> 2, anything else -> 0 (a0 on all 64 bits). 1 call site.
#include "theseus.h"

void Map1234_001711c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    switch (g.sarg64(0))
    {
    case 1: g.ret(1); break;
    case 2: g.ret(4); break;
    case 3: g.ret(3); break;
    case 4: g.ret(2); break;
    default: g.ret(0); break;
    }
}

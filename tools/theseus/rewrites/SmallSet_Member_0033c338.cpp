// 1 if a0 (all 64 bits) is one of 1, 3, 5, 6, 7, 8, 9, 15, 22, 23, else 0. 1 call site.
#include "theseus.h"

void SmallSet_Member_0033c338(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    switch (g.sarg64(0))
    {
    case 1: case 3: case 5: case 6: case 7: case 8: case 9: case 15: case 22: case 23:
        g.ret(1);
        break;
    default:
        g.ret(0);
        break;
    }
}

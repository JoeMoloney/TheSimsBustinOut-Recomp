// Returns the address 0x410390 (a global object). 2 call sites.
#include "theseus.h"

void Const_00410390_001d3c88(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(0x00410390);
}

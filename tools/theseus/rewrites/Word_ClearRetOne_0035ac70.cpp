// Zeroes the word at a0 and returns 1. 6 call sites (3 identical copies).
// theseus: copies 0x365498, 0x365508
#include "theseus.h"

void Word_ClearRetOne_0035ac70(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0);
    g.ret(1);
}

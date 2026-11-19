// Absolute value of a double passed and returned in a 64-bit register: clears bit 63 of a0
// (all 64 bits of v0 are returned). 5 call sites.
#include "theseus.h"

void Double_Abs_0022e680(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret64(g.arg64(0) & 0x7FFFFFFFFFFFFFFFull);
}

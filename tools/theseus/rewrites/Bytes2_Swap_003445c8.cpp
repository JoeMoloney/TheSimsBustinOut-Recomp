// Swaps the bytes at a0 and a0+1 (a 16-bit byte swap in place). v0 = the old first byte,
// sign-extended. 4 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Bytes2_Swap_003445c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const int8_t b0 = g.sread8(p), b1 = g.sread8(p + 1);  // gate: equivalent read8: stored in an int8_t either way
    g.write8(p + 1, static_cast<uint8_t>(b0));
    g.write8(p, static_cast<uint8_t>(b1));
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(b0)));
}

// The 16-bit value at a0 with its top bit cleared, or -1 if it is 0xFFFF (15 call sites).
#include "theseus.h"

void Short15_OrMinus1_00347650(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint16_t v = g.read16(g.arg(0));  // gate: equivalent read32: stored in 16 bits
    g.ret(v == 0xFFFF ? 0xFFFFFFFFu : v & 0x7FFFu);
}

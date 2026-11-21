// Subtracts the two bytes at a1 from the two at a0, in place (byte-wise, wrapping), with the original'\''s read/write order. Returns a0. 2 call sites.
#include "theseus.h"

void Bytes2_Subtract_002cc168(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0), b = g.arg(1);
    const uint8_t b0 = g.read8(b), a0v = g.read8(a), a1v = g.read8(a + 1);  // gate: equivalent read16: kept as bytes
    g.write8(a, static_cast<uint8_t>(a0v - b0));  // gate: equivalent write16: a+1 is stored next (a and b never overlap at the call sites)
    g.write8(a + 1, static_cast<uint8_t>(a1v - g.read8(b + 1)));  // gate: equivalent read16: kept as bytes
    g.ret_copy_of_arg(0);
}

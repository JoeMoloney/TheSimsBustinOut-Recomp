// Number of 4-byte elements between +0x8 (begin) and +0xC (end) of a0, (end - begin) >> 2
// as a signed 32-bit value (9 call sites).
#include "theseus.h"

void Vector_Count4_00347638(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(g.read32(a + 0xC) - g.read32(a + 0x8)) >> 2));
}

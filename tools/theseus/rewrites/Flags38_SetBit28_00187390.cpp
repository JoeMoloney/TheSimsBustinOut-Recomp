// Set bit 28 (0x10000000) of the word at +0x38 and return the new value (28 callers).
// Replaces FUN_00187390. What the flag means is not known yet.
#include "theseus.h"

void Flags38_SetBit28_00187390(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x38;
    const uint32_t value = g.read32(field) | 0x10000000u;
    g.write32(field, value);
    g.ret(value);
}

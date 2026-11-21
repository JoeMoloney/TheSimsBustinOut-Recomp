// Zeroes the word at a1, then decrements the word at a0+0x4500 and returns the new value. 2 call sites.
#include "theseus.h"

void Field4500_DecrementClear_00231d80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(1), 0);
    const uint32_t field = g.arg(0) + 0x4500, value = g.read32(field) - 1;
    g.write32(field, value);
    g.ret(value);
}

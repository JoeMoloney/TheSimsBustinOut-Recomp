// Decrements the word at a0+0xFC and returns the new value. 5 call sites.
#include "theseus.h"

void FieldFC_Decrement_002ea6d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(g.arg(0) + 0xFC) - 1;
    g.write32(g.arg(0) + 0xFC, value);
    g.ret(value);
}

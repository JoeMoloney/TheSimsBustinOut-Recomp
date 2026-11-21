// Sets bit 8 (0x100) of the word at a0+0x20; returns the new word. 2 call sites.
#include "theseus.h"

void Field20_SetBit8_00341d30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x20, value = g.read32(field) | 0x100;
    g.write32(field, value);  // gate: equivalent write16: bit 8 is in the low half
    g.ret(value);
}

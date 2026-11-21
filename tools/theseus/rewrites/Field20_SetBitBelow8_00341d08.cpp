// Sets bit a1 of the word at a0+0x20 when a1 < 8 (unsigned, whole register); returns the mask (1 when a1 is out of range). 3 call sites.
#include "theseus.h"

void Field20_SetBitBelow8_00341d08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) < 8)
    {
        const uint32_t field = g.arg(0) + 0x20, bit = 1u << g.arg(1);
        g.write32(field, g.read32(field) | bit);  // gate: equivalent write16: bits 0-7 only; the upper half stays
        g.ret(bit);
        return;
    }
    g.ret(1);
}

// Copies the word at a0+8 to a0+0xC and zeroes the 7 words a0+0x58 down to a0+0x40. Returns
// the copied word (left in v0). 8 call sites.
#include "theseus.h"

void Obj_ResetC_40to58_001ffac8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0), value = g.read32(obj + 8);
    g.write32(obj + 0xC, value);
    for (uint32_t off = 0x58; off >= 0x40; off -= 4)
        g.write32(obj + off, 0);
    g.ret(value);
}

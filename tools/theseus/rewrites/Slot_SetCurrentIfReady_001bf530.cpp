// a1 = slot index: stores it in the global 0x3F8B90 when the slot's three flag words
// (0x40B310/0x40B320/0x40B330 + a1*4) are all nonzero, or else when the global 0x4106F4 is
// nonzero or a1 == 1. v0 is left as the original leaves it: the third flag's address when all
// three flags are set, else 1. 7 call sites.
#include "theseus.h"

void Slot_SetCurrentIfReady_001bf530(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t slot = g.arg(1), off = slot << 2;
    if (g.read32(0x40B310 + off) != 0 && g.read32(0x40B320 + off) != 0 && g.read32(0x40B330 + off) != 0)
    {
        g.write32(0x3F8B90, slot);
        g.ret(0x40B330 + off);
        return;
    }
    if (g.read32(0x4106F4) != 0 || g.arg64(1) == 1)
        g.write32(0x3F8B90, slot);
    g.ret(1);
}

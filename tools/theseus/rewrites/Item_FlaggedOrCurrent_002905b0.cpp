// 1 if the item a1 has a non-zero word at +0xC or +8, or is (all 64 bits) the current item
// [a0+0x9C]; else 0. 1 call site.
#include "theseus.h"

void Item_FlaggedOrCurrent_002905b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t item = g.arg(1);
    const bool flagged = g.read32(item + 0xC) != 0 || g.read32(item + 8) != 0;
    g.ret(flagged || g.sarg64(1) == static_cast<int32_t>(g.read32(g.arg(0) + 0x9C)) ? 1 : 0);
}

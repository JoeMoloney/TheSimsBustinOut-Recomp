// Moves the rectangle {left +0, top +4, right +8, bottom +0xC} at a0 by (a1, a2), each taken as a signed 16-bit value. v0 = the new right. 3 call sites. (The map joined other code after the `jr $ra`.)
#include "theseus.h"

void Rect_Offset_00336378(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t r = g.arg(0);
    const uint32_t dx = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(g.arg(1))));
    const uint32_t dy = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(g.arg(2))));
    const uint32_t top = g.read32(r + 4) + dy, bottom = g.read32(r + 0xC) + dy;
    const uint32_t left = g.read32(r) + dx, right = g.read32(r + 8) + dx;
    g.write32(r + 4, top);
    g.write32(r + 8, right);
    g.write32(r + 0xC, bottom);
    g.write32(r, left);
    g.ret(right);
}
